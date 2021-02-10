//

#include <mpi.h>

#include "Config.hpp"
#include "MPIData.hpp"
#include "Model.hpp"

#include "gtest/gtest.h"

class MPIObjTest : public ::testing::Test{
    protected:
    //myTest(){};
    //~myTest(){};
    
    void SetUp() override{
        MPI_Comm_dup(MPI_COMM_WORLD,&mycomm);
        MPI_Comm_size(MPI_COMM_WORLD,&mycomm_size);
        MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
        for(int k=0;k<mycomm_size;k++){
            values.push_back(k);
        };
        lneighbor=((myrank-1)>=0)?(myrank-1):MPI_PROC_NULL;
        rneighbor=((myrank+1)<mycomm_size)?(myrank+1):MPI_PROC_NULL;

        numTS = 46250;
        stepSize = 0.0000000032453;
        numMat = 1;
        numExc = 1;
        modParseNames.clear();
        modParseNames.push_back("time");
        modParseNames.push_back("geometry");
        modParseNames.push_back("materials");
        modParseNames.push_back("excitations");
        memcpy(&infilename,  "QuasiIsoPlateSimulationInputs.xml", sizeof(char)*50);
        memcpy(&infilepath, testInputFileDirectory, sizeof(char)*150);

        dimensions.clear();
        dimensions.push_back(122);
        dimensions.push_back(122);
        dimensions.push_back(26);

        voxSz.clear();
        voxSz.push_back(0.00025);
        voxSz.push_back(0.00025);
        voxSz.push_back(0.00004);
        geomfile = "Geometry";
        rotfile = "Rotations.bin";

        matIDnumber = 1; 
        density = 1570.0;
        stiffnessValuesVec.clear();
        stiffnessValuesVec.push_back(175200000000.0);
        stiffnessValuesVec.push_back(5940000000.0);
        stiffnessValuesVec.push_back(5940000000.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(12300000000.0);
        stiffnessValuesVec.push_back(6254000000.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(12300000000.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(2800000000.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(5290000000.0);
        stiffnessValuesVec.push_back(0.0);
        stiffnessValuesVec.push_back(5290000000.0);

        excIDnumber = 1;
        radius = 0.0025;
        drivn = true; 
        initT = 0;
        excnumTS = 46221;
        location.clear();
        location.push_back(0.01525);
        location.push_back(0.01525);
        location.push_back(0.00004);
        excFile = "Excitation.bin";
        scaling.clear();
        scaling.push_back(0.0);
        scaling.push_back(0.0);
        scaling.push_back(1.0);
        norm.clear();
        norm.push_back(0.0);
        norm.push_back(0.0);
        norm.push_back(1.0);

    };
    //void TearDown() override{};
    
    MPI_Comm mycomm;
    int mycomm_size,myrank;
    std::vector<int> values;
    int recieved[2]={-1,-1};
    int lneighbor,rneighbor;
    MPI_Status status[2];

    uint32_t numTS;
    double stepSize;
    uint8_t numMat; 
    uint8_t numExc;
    char infilename[50];
    char infilepath[150];
    std::vector<std::string> modParseNames; 

    std::vector<double> voxSz;
    std::vector<uint64_t> dimensions;
    std::string geomfile; 
    std::string rotfile;

    uint32_t matIDnumber;
    float density; 
    std::vector<float> stiffnessValuesVec;

    uint32_t excIDnumber;
    double radius;
    bool drivn; 
    uint32_t initT;
    uint32_t excnumTS;
    std::vector<double> location;
    std::string excFile; 
    std::vector<float> scaling;
    std::vector<float> norm;

};


TEST_F(MPIObjTest,MPISendRecieveTest){
    swSim::Model mod;
    std::string inputFile = std::string(testInputFileDirectory) 
    + std::string("30x30ElongatedQausiIsoCFRPTestPlate/") 
    + std::string("QuasiIsoPlateSimulationInputs.xml");
    
    mod.Input((char*)inputFile.c_str(), mycomm);
    
    EXPECT_EQ(1, 1);

    EXPECT_EQ(mod.modS.numTimeSteps, numTS);
    EXPECT_EQ(mod.modS.dt, stepSize);
    EXPECT_EQ(mod.modS.numMaterials, numMat);
    EXPECT_EQ(mod.modS.numExcitations, numExc);
    for(int i = 0; i<modParseNames.size(); i++) 
    {
       EXPECT_EQ(mod.parseNames[i], modParseNames[i]);  
    }
    EXPECT_EQ(mod.modMaterialsList.size(), numMat);
    EXPECT_EQ(mod.modExcitationsList.size(), numExc);
    EXPECT_STREQ(infilename,mod.modS.inputFile);
    // EXPECT_STREQ(infilepath,mod.modS.inputPath);

    EXPECT_EQ(mod.modGeometry.geomS.dimensionsGlobalVoxel[0], dimensions[0]);
    EXPECT_EQ(mod.modGeometry.geomS.dimensionsGlobalVoxel[1], dimensions[1]);
    EXPECT_EQ(mod.modGeometry.geomS.dimensionsGlobalVoxel[2], dimensions[2]);
    EXPECT_EQ(mod.modGeometry.geomS.voxelSize[0], voxSz[0]);
    EXPECT_EQ(mod.modGeometry.geomS.voxelSize[1], voxSz[1]);
    EXPECT_EQ(mod.modGeometry.geomS.voxelSize[2], voxSz[2]);
    EXPECT_STREQ(mod.modGeometry.geomS.geometryFile, geomfile.c_str());
    EXPECT_STREQ(mod.modGeometry.geomS.rotationsFile, rotfile.c_str());

    EXPECT_EQ(mod.modMaterialsList.at(0).matS.materialID, matIDnumber);
    EXPECT_EQ(mod.modMaterialsList.at(0).matS.density, density);
    for(int i = 0; i<21; i++) 
    {
        EXPECT_EQ(mod.modMaterialsList.at(0).matS.stiffnessValues[i], stiffnessValuesVec[i]);
    }

    EXPECT_EQ(mod.modExcitationsList.at(0).excS.excitationID, excIDnumber);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.location[0], location[0]);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.location[1], location[1]);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.location[2], location[2]);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.radius, radius);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.driven, drivn);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.initTimeStep, initT);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.excitationNumTimeSteps, excnumTS);
    EXPECT_STREQ(mod.modExcitationsList.at(0).excS.waveformFile, excFile.c_str());
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.scalingVector[0], scaling[0]);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.scalingVector[1], scaling[1]);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.scalingVector[2], scaling[2]);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.orientNormal[0], norm[0]);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.orientNormal[1], norm[1]);
    EXPECT_EQ(mod.modExcitationsList.at(0).excS.orientNormal[2], norm[2]);
     
};