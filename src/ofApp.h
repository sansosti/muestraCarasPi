#pragma once

#include "ofMain.h"
#include "ofxMtlWatchFolder.h"
#include "ofxXmlSettings.h"

#define ABSOLUTE_MAX_CARAS 75

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

    protected:
		int totalCaras;
		string carasFileNames[ABSOLUTE_MAX_CARAS];
		ofImage caras[ABSOLUTE_MAX_CARAS];
		ofPoint posicionCaras[ABSOLUTE_MAX_CARAS];
		ofPoint velocidadCaras[ABSOLUTE_MAX_CARAS];
		ofPoint aceleracionCaras[ABSOLUTE_MAX_CARAS];
		ofPoint viento;

		int currentCara;

        void watchFolderFileAdded(string& filename);
        void watchFolderFileRemoved(string& filename);
        void eliminarCara(string filename);

        int getFilenameIndex(string filename);

        ofxXmlSettings XML;
        void loadConfig();
        bool debug;
        int alpha;
        int maxCaras;

        int watchFolderRefresh;

        ofPoint aceleracionNormal;
        ofPoint aceleracionBordes;

    private:
        ofxMtlWatchFolder watchFolder;

};
