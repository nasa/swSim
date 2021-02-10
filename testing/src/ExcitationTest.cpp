//

#include <string>
#include <vector>
#include <cstdio>

#include "Config.hpp"
#include "Excitation.hpp"

#include "gtest/gtest.h"

class ExcitationTest: public ::testing::Test{
    protected:
    //DenseVectorTest(){};
    //~DenseVectorTest(){};
    
    void SetUp() override{
        inputFile = std::string(testInputFileDirectory) + std::string("30x30ElongatedQausiIsoCFRPTestPlate/") + std::string("QuasiIsoPlateSimulationInputs.xml");
        doc = xmlParseFile(inputFile.c_str());
        cur = xmlDocGetRootElement(doc);

        IDnumber = 1;
        radius = 0.0025;
        drivn = true; 
        initT = 0;
        numTS = 46221;
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
        excProc = 2;
        cElement = 206754;
        eElement = 206938;
        excParseNames.clear();
        excParseNames.push_back("number");
        excParseNames.push_back("location");
        excParseNames.push_back("radius");
        excParseNames.push_back("driven");
        excParseNames.push_back("initTimeStep");
        excParseNames.push_back("numTimeSteps");
        excParseNames.push_back("waveformFile");
        excParseNames.push_back("scalingVector");
        excParseNames.push_back("orientation");
    };
    //void TearDown() override{};
    xmlDocPtr doc;
    xmlNodePtr cur;
    std::string inputFile;

    uint32_t IDnumber;
    double radius;
    bool drivn; 
    uint32_t initT;
    uint32_t numTS;
    std::vector<double> location;
    std::string excFile; 
    std::vector<float> scaling;
    std::vector<float> norm;
    int excProc;
    uint64_t cElement;
    uint64_t eElement;
    std::vector<std::string> excParseNames; 
};

TEST_F(ExcitationTest,Initializer){
    swSim::Excitation exc;
    exc.Init(IDnumber, &location[0], radius, drivn, initT, numTS, excFile.c_str(), &scaling[0], &norm[0]);
    EXPECT_EQ(exc.excS.excitationID, IDnumber);
    EXPECT_EQ(exc.excS.location[0], location[0]);
    EXPECT_EQ(exc.excS.location[1], location[1]);
    EXPECT_EQ(exc.excS.location[2], location[2]);
    EXPECT_EQ(exc.excS.radius, radius);
    EXPECT_EQ(exc.excS.driven, drivn);
    EXPECT_EQ(exc.excS.initTimeStep, initT);
    EXPECT_EQ(exc.excS.excitationNumTimeSteps, numTS);
    EXPECT_STREQ(exc.excS.waveformFile, excFile.c_str());
    EXPECT_EQ(exc.excS.scalingVector[0], scaling[0]);
    EXPECT_EQ(exc.excS.scalingVector[1], scaling[1]);
    EXPECT_EQ(exc.excS.scalingVector[2], scaling[2]);
    EXPECT_EQ(exc.excS.orientNormal[0], norm[0]);
    EXPECT_EQ(exc.excS.orientNormal[1], norm[1]);
    EXPECT_EQ(exc.excS.orientNormal[2], norm[2]);

    for(int i = 0; i < excParseNames.size(); i++) {
        EXPECT_EQ(exc.parseNames[i], excParseNames[i]); 
    }
    
};


TEST_F(ExcitationTest,Copier){
    swSim::Excitation exc1;
    swSim::Excitation exc2;
    exc1.Init(IDnumber, &location[0], radius, drivn, initT, numTS, excFile.c_str(), &scaling[0], &norm[0]);
    exc2.Copy(&exc1);
    EXPECT_EQ(exc2.excS.excitationID, IDnumber);
    EXPECT_EQ(exc2.excS.location[0], location[0]);
    EXPECT_EQ(exc2.excS.location[1], location[1]);
    EXPECT_EQ(exc2.excS.location[2], location[2]);
    EXPECT_EQ(exc2.excS.radius, radius);
    EXPECT_EQ(exc2.excS.driven, drivn);
    EXPECT_EQ(exc2.excS.initTimeStep, initT);
    EXPECT_EQ(exc2.excS.excitationNumTimeSteps, numTS);
    EXPECT_STREQ(exc2.excS.waveformFile, excFile.c_str());
    EXPECT_EQ(exc2.excS.scalingVector[0], scaling[0]);
    EXPECT_EQ(exc2.excS.scalingVector[1], scaling[1]);
    EXPECT_EQ(exc2.excS.scalingVector[2], scaling[2]);
    EXPECT_EQ(exc2.excS.orientNormal[0], norm[0]);
    EXPECT_EQ(exc2.excS.orientNormal[1], norm[1]);
    EXPECT_EQ(exc2.excS.orientNormal[2], norm[2]);
};

TEST_F(ExcitationTest,LocationCheckTest1){
    swSim::Excitation exc1;
    exc1.Init(IDnumber, &location[0], radius, drivn, initT, numTS, excFile.c_str(), &scaling[0], &norm[0]);
    std::vector<double> loc1{0.01525, 0.01525, 0.00004};
    bool loc = exc1.LocationCheck(loc1);
    EXPECT_EQ(loc, true);
    loc1[0] = 0.015;
    loc = exc1.LocationCheck(loc1);
    EXPECT_EQ(loc, true);
    loc1[0] = 0.01525;
    loc1[1] = 0.016;
    loc = exc1.LocationCheck(loc1);
    EXPECT_EQ(loc, true);
    loc1[1] = 0.01525,
    loc1[2] = 0.000045;
    loc = exc1.LocationCheck(loc1);
    EXPECT_EQ(loc, true);
    loc1[0] = 0.01525;
    loc1[1] = 0.01525;
    loc1[2] = 0.10004;
    loc = exc1.LocationCheck(loc1);
    EXPECT_EQ(loc, false);
    loc1[0] = 0.500;
    loc1[1] = 0.100;
    loc1[2] = 0.00004;
    loc = exc1.LocationCheck(loc1);
    EXPECT_EQ(loc, false);
    loc1[0] = 0.500;
    loc1[1] = 0.200;
    loc1[2] = 0.007;
    loc = exc1.LocationCheck(loc1);
    EXPECT_EQ(loc, false);

};

TEST_F(ExcitationTest,ParseSearchTest){
    swSim::Excitation exc;
    cur = xmlDocGetRootElement(doc);
    cur = exc.ParseSearch(doc, cur, "excitations");
    cur = exc.ParseSearch(doc, cur, "excitation");
    xmlNodePtr cur2;
    char keystr[50]; // The xml tag character container
    for (int i = 0; i < excParseNames.size(); i++) {
        cur2 = exc.ParseSearch(doc, cur, excParseNames[i].c_str());
        sprintf(keystr, "%s", cur2->name);
        EXPECT_STREQ(keystr, excParseNames[i].c_str());
    }
};

TEST_F(ExcitationTest,ParseTest){
    xmlNodePtr cur2;
    swSim::Excitation exc;

    cur = xmlDocGetRootElement(doc);
    cur2 = exc.ParseSearch(doc, cur, "excitations");
    cur2 = exc.ParseSearch(doc, cur2, "excitation");
    exc.Parse(doc, cur2); 

    for(int i = 0; i < norm.size(); i++) {
        EXPECT_EQ(exc.excS.location[i], location[i]);
        EXPECT_EQ(exc.excS.scalingVector[i], scaling[i]);
        EXPECT_EQ(exc.excS.orientNormal[i], norm[i]);
    }
    EXPECT_EQ(exc.excS.excitationID, IDnumber);
    EXPECT_EQ(exc.excS.radius, radius);
    EXPECT_EQ(exc.excS.driven, drivn);
    EXPECT_EQ(exc.excS.initTimeStep, initT);
    EXPECT_EQ(exc.excS.excitationNumTimeSteps, numTS);
    EXPECT_STREQ(exc.excS.waveformFile,excFile.c_str());
    
};
