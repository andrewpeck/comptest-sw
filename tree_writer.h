#ifndef TEST_WRITER_H
#define TEST_WRITER_H


class treeWriter {

    public:

        treeWriter() {
            //spawnTrees();
        };

        ~treeWriter() {
            // delete hfile;
            // delete thresholds_r;
            // delete thresholds_l;
            // delete offsets_r;
            // delete offsets_l;
            // delete currents;
        };


        void fillThresh(int strip, int side, uint16_t* data);
        void fillOffset(int strip, int side, uint16_t* data);
        void fillGeneric(int scan, int strip, int side, uint16_t* data);

        void fillCurrent(int channel, uint16_t* data);

        void spawnTrees ();

    private:


        TFile *hfile;

        TTree *thresholds_r;
        TTree *thresholds_l;
        TTree *offsets_r;
        TTree *offsets_l;
        TTree *currents;

};

#endif /* TEST_WRITER_H */
