// indiv.cpp

#include <iostream>
#include <cassert>

#include "../include/indiv.h"
#include "../../simulator.h"
// #include "../../dirFactory.h"
#include "./genome.cpp"
#include "../include/connectionList.h"
#include "../../getSensor.h"
// #include "../../random.h"

namespace BS {

    // Indiv::Indiv(std::unique_ptr<RandomUintGenerator> r) : randomUint {r}
    // {

    // }

    // Indiv::~Indiv()
    // {

    // }

// This is called when any individual is spawned.
// The responsiveness parameter will be initialized here to maximum value
// of 1.0, then depending on which action activation function is used,
// the default undriven value may be changed to 1.0 or action midrange.
void Indiv::initialize(uint16_t index_, Coord loc_, Genome &&genome_, Dir dir)
{
    index = index_;
    loc = loc_;
    //birthLoc = loc_;
    grid.set(loc_, index_);
    age = 0;
    oscPeriod = 34; // ToDo !!! define a constant
    alive = true;
    // TODO: pass the lastMoveDir as a parameter
    lastMoveDir = dir;
    // randomUint = r;

    responsiveness = 0.5; // range 0.0..1.0
    // TODO: pass the longProbeDist as a parameter
    longProbeDist = p.longProbeDistance;
    challengeBits = (unsigned)false; // will be set true when some task gets accomplished
    genome = std::move(genome_);
    createWiringFromGenome();
}

// This function is used when an agent is spawned. This function converts the
// agent's inherited genome into the agent's neural net brain. There is a close
// correspondence between the genome and the neural net, but a connection
// specified in the genome will not be represented in the neural net if the
// connection feeds a neuron that does not itself feed anything else.
// Neurons get renumbered in the process:
// 1. Create a set of referenced neuron numbers where each index is in the
//    range 0..p.genomeMaxLength-1, keeping a count of outputs for each neuron.
// 2. Delete any referenced neuron index that has no outputs or only feeds itself.
// 3. Renumber the remaining neurons sequentially starting at 0.
void Indiv::createWiringFromGenome()
{
    NodeMap nodeMap;  // list of neurons and their number of inputs and outputs
    ConnectionList connectionList(p.maxNumberNeurons, nodeMap); // synaptic connections

    // Convert the indiv's genome to a renumbered connection list
    connectionList.renumber(genome);

    // Make a node (neuron) list from the renumbered connection list
    connectionList.makeNodeList();

    // Find and remove neurons that don't feed anything or only feed themself.
    // This reiteratively removes all connections to the useless neurons.
    connectionList.cullUselessNeurons();

    // The neurons map now has all the referenced neurons, their neuron numbers, and
    // the number of outputs for each neuron. Now we'll renumber the neurons
    // starting at zero.

    assert(nodeMap.size() <= p.maxNumberNeurons);
    uint16_t newNumber = 0;
    for (auto & node : nodeMap) {
        assert(node.second.numOutputs != 0);
        node.second.remappedNumber = newNumber++;
    }

    // Create the indiv's connection list in two passes:
    // First the connections to neurons, then the connections to actions.
    // This ordering optimizes the feed-forward function in feedForward.cpp.

    nnet.connections.clear();

    // First, the connections from sensor or neuron to a neuron
    for (auto const &conn : connectionList.connections()) {

        if (conn.sinkType == NEURON) {
            nnet.connections.push_back(conn);
            auto &newConn = nnet.connections.back();
            // fix the destination neuron number
            newConn.sinkNum = nodeMap[newConn.sinkNum].remappedNumber;
            // if the source is a neuron, fix its number too
            if (newConn.sourceType == NEURON) {
                newConn.sourceNum = nodeMap[newConn.sourceNum].remappedNumber;
            }
        }
    }

    // Last, the connections from sensor or neuron to an action
    for (auto const &conn : connectionList.connections()) {
        if (conn.sinkType == ACTION) {
            nnet.connections.push_back(conn);
            auto &newConn = nnet.connections.back();
            // if the source is a neuron, fix its number
            if (newConn.sourceType == NEURON) {
                newConn.sourceNum = nodeMap[newConn.sourceNum].remappedNumber;
            }
        }
    }

    // Create the indiv's neural node list
    nnet.neurons.clear();
    for (unsigned neuronNum = 0; neuronNum < nodeMap.size(); ++neuronNum) {
        nnet.neurons.push_back( {} );
        nnet.neurons.back().output = initialNeuronOutput();
        nnet.neurons.back().driven = (nodeMap[neuronNum].numInputsFromSensorsOrOtherNeurons != 0);
    }
}

// Returned sensor values range SENSOR_MIN..SENSOR_MAX
float Indiv::getSensor(Sensor sensorNum, unsigned simStep, unsigned genomeComparisonMethod) const
{
    float sensorVal = 0.0;

    switch (sensorNum) {
    case Sensor::AGE:
        // Converts age (units of simSteps compared to life expectancy)
        // linearly to normalized sensor range 0.0..1.0
        sensorVal = (float)age / p.stepsPerGeneration;
        break;
    case Sensor::BOUNDARY_DIST:
    {
        // Finds closest boundary, compares that to the max possible dist
        // to a boundary from the center, and converts that linearly to the
        // sensor range 0.0..1.0
        int distX = std::min<int>(loc.x, (p.sizeX - loc.x) - 1);
        int distY = std::min<int>(loc.y, (p.sizeY - loc.y) - 1);
        int closest = std::min<int>(distX, distY);
        int maxPossible = std::max<int>(p.sizeX / 2 - 1, p.sizeY / 2 - 1);
        sensorVal = (float)closest / maxPossible;
        break;
    }
    case Sensor::BOUNDARY_DIST_X:
    {
        // Measures the distance to nearest boundary in the east-west axis,
        // max distance is half the grid width; scaled to sensor range 0.0..1.0.
        int minDistX = std::min<int>(loc.x, (p.sizeX - loc.x) - 1);
        sensorVal = minDistX / (p.sizeX / 2.0);
        break;
    }
    case Sensor::BOUNDARY_DIST_Y:
    {
        // Measures the distance to nearest boundary in the south-north axis,
        // max distance is half the grid height; scaled to sensor range 0.0..1.0.
        int minDistY = std::min<int>(loc.y, (p.sizeY - loc.y) - 1);
        sensorVal = minDistY / (p.sizeY / 2.0);
        break;
    }
    case Sensor::LAST_MOVE_DIR_X:
    {
        // X component -1,0,1 maps to sensor values 0.0, 0.5, 1.0
        auto lastX = lastMoveDir.asNormalizedCoord().x;
        sensorVal = lastX == 0 ? 0.5 :
                (lastX == -1 ? 0.0 : 1.0);
        break;
    }
    case Sensor::LAST_MOVE_DIR_Y:
    {
        // Y component -1,0,1 maps to sensor values 0.0, 0.5, 1.0
        auto lastY = lastMoveDir.asNormalizedCoord().y;
        sensorVal = lastY == 0 ? 0.5 :
                (lastY == -1 ? 0.0 : 1.0);
        break;
    }
    case Sensor::LOC_X:
        // Maps current X location 0..p.sizeX-1 to sensor range 0.0..1.0
        sensorVal = (float)loc.x / (p.sizeX - 1);
        break;
    case Sensor::LOC_Y:
        // Maps current Y location 0..p.sizeY-1 to sensor range 0.0..1.0
        sensorVal = (float)loc.y / (p.sizeY - 1);
        break;
    case Sensor::OSC1:
    {
        // Maps the oscillator sine wave to sensor range 0.0..1.0;
        // cycles starts at simStep 0 for everbody.
        float phase = (simStep % oscPeriod) / (float)oscPeriod; // 0.0..1.0
        float factor = -std::cos(phase * 2.0f * 3.1415927f);
        assert(factor >= -1.0f && factor <= 1.0f);
        factor += 1.0f;    // convert to 0.0..2.0
        factor /= 2.0;     // convert to 0.0..1.0
        sensorVal = factor;
        // Clip any round-off error
        sensorVal = std::min<float>(1.0, std::max<float>(0.0, sensorVal));
        break;
    }
    case Sensor::LONGPROBE_POP_FWD:
    {
        // Measures the distance to the nearest other individual in the
        // forward direction. If non found, returns the maximum sensor value.
        // Maps the result to the sensor range 0.0..1.0.
        sensorVal = grid.longProbePopulationFwd(loc, lastMoveDir, longProbeDist) / (float)longProbeDist; // 0..1
        break;
    }
    case Sensor::LONGPROBE_BAR_FWD:
    {
        // Measures the distance to the nearest barrier in the forward
        // direction. If non found, returns the maximum sensor value.
        // Maps the result to the sensor range 0.0..1.0.
        sensorVal = grid.longProbeBarrierFwd(loc, lastMoveDir, longProbeDist) / (float)longProbeDist; // 0..1
        break;
    }
    case Sensor::POPULATION:
    {
        // Returns population density in neighborhood converted linearly from
        // 0..100% to sensor range
        unsigned countLocs = 0;
        unsigned countOccupied = 0;
        Coord center = loc;

        auto f = [&](Coord tloc) {
            ++countLocs;
            if (grid.isOccupiedAt(tloc)) {
                ++countOccupied;
            }
        };

        visitNeighborhood(center, p.populationSensorRadius, f);
        sensorVal = (float)countOccupied / countLocs;
        break;
    }
    case Sensor::POPULATION_FWD:
        // Sense population density along axis of last movement direction, mapped
        // to sensor range 0.0..1.0
        sensorVal = getPopulationDensityAlongAxis(loc, lastMoveDir);
        break;
    case Sensor::POPULATION_LR:
        // Sense population density along an axis 90 degrees from last movement direction
        sensorVal = getPopulationDensityAlongAxis(loc, lastMoveDir.rotate90DegCW());
        break;
    case Sensor::BARRIER_FWD:
        // Sense the nearest barrier along axis of last movement direction, mapped
        // to sensor range 0.0..1.0
        sensorVal = grid.getShortProbeBarrierDistance(loc, lastMoveDir, p.shortProbeBarrierDistance);
        break;
    case Sensor::BARRIER_LR:
        // Sense the nearest barrier along axis perpendicular to last movement direction, mapped
        // to sensor range 0.0..1.0
        sensorVal = grid.getShortProbeBarrierDistance(loc, lastMoveDir.rotate90DegCW(), p.shortProbeBarrierDistance);
        break;
        
        // TODO: replace later
    // case Sensor::RANDOM:
    //     // Returns a random sensor value in the range 0.0..1.0.
    //     sensorVal = randomUint->operator()() / (float)UINT_MAX;
    //     break;
    case Sensor::SIGNAL0:
        // Returns magnitude of signal0 in the local neighborhood, with
        // 0.0..maxSignalSum converted to sensorRange 0.0..1.0
        sensorVal = getSignalDensity(0, loc);
        break;
    case Sensor::SIGNAL0_FWD:
        // Sense signal0 density along axis of last movement direction
        sensorVal = getSignalDensityAlongAxis(0, loc, lastMoveDir);
        break;
    case Sensor::SIGNAL0_LR:
        // Sense signal0 density along an axis perpendicular to last movement direction
        sensorVal = getSignalDensityAlongAxis(0, loc, lastMoveDir.rotate90DegCW());
        break;
    case Sensor::GENETIC_SIM_FWD:
    {
        // Return minimum sensor value if nobody is alive in the forward adjacent location,
        // else returns a similarity match in the sensor range 0.0..1.0
        Coord loc2 = loc + lastMoveDir;
        if (grid.isInBounds(loc2) && grid.isOccupiedAt(loc2)) {
            const Indiv &indiv2 = peeps.getIndiv(loc2);
            if (indiv2.alive) {
                sensorVal = genomeSimilarity(genome, indiv2.genome, genomeComparisonMethod); // 0.0..1.0
            }
        }
        break;
    }
    default:
        assert(false);
        break;
    }

    if (std::isnan(sensorVal) || sensorVal < -0.01 || sensorVal > 1.01) {
        std::cout << "sensorVal=" << (int)sensorVal << " for " << sensorName((Sensor)sensorNum) << std::endl;
        sensorVal = std::max(0.0f, std::min(sensorVal, 1.0f)); // clip
    }

    assert(!std::isnan(sensorVal) && sensorVal >= -0.01 && sensorVal <= 1.01);

    return sensorVal;
}

} // end namespace BS
