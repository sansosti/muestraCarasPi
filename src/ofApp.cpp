#include "ofApp.h"

const string ConfigFile = "config.xml";
const ofPoint maxVelInicial(0.0001,0.0001);
const ofPoint maxVelocidad(0.03,0.03);

#define CARAS_PATH "caras"

#define DEFAULT_DEBUG 0
#define DEFAULT_ALPHA 127
#define DEFAULT_WATCH_FOLDER_REFRESH 2000

#define DEFAULT_ACELERACION_NORMAL_X 0.0000001
#define DEFAULT_ACELERACION_NORMAL_Y 0.0000001
#define DEFAULT_ACELERACION_BORDES_X 0.0001
#define DEFAULT_ACELERACION_BORDES_Y 0.0001

//--------------------------------------------------------------
void ofApp::setup(){
    ofHideCursor();
    ofBackground(0,0,0);

    loadConfig();

    totalCaras = 0;
    currentCara = -1;

    viento = ofPoint(0.000001,-0.000001);

    // start watching the bin/data/watch/ folder every 2000 ms (2 secs)
    watchFolder.start(CARAS_PATH, watchFolderRefresh);
    ofAddListener(watchFolder.fileAdded, this, &ofApp::watchFolderFileAdded);
    ofAddListener(watchFolder.fileRemoved, this, &ofApp::watchFolderFileRemoved);

}
//--------------------------------------------------------------
void ofApp::loadConfig() {
    if( XML.loadFile(ConfigFile) ){
		ofLogNotice() << "Cargando configuracion desde '" + ConfigFile + "'";
	} else {
		ofLogNotice() << "Archivo de configuracion '" + ConfigFile + "' no encontrado";
		//return;
	}

    int debugInt = XML.getValue("MUESTRACARAS:DEBUG",DEFAULT_DEBUG);
    debug = (debugInt == 1);
    maxCaras = XML.getValue("MUESTRACARAS:MAX_CARAS",ABSOLUTE_MAX_CARAS);
    if (maxCaras > ABSOLUTE_MAX_CARAS) {
        ofLogNotice("maxCaras demasiado alto: " + ofToString(maxCaras) + " - Truncado a " + ofToString(ABSOLUTE_MAX_CARAS));
        maxCaras = ABSOLUTE_MAX_CARAS;
    }
	alpha        = XML.getValue("MUESTRACARAS:ALPHA",DEFAULT_ALPHA);
	watchFolderRefresh  = XML.getValue("MUESTRACARAS:WATCH_FOLDER_REFRESH",DEFAULT_WATCH_FOLDER_REFRESH);

	aceleracionNormal = ofPoint(XML.getValue("MUESTRACARAS:ACELERACION_NORMAL:X",DEFAULT_ACELERACION_NORMAL_X),
                           XML.getValue("MUESTRACARAS:ACELERACION_NORMAL:Y",DEFAULT_ACELERACION_NORMAL_Y));
	aceleracionBordes = ofPoint(XML.getValue("MUESTRACARAS:ACELERACION_BORDES:X",DEFAULT_ACELERACION_BORDES_X),
                           XML.getValue("MUESTRACARAS:ACELERACION_BORDES:Y",DEFAULT_ACELERACION_BORDES_Y));

    ofLogNotice() << "maxCaras: " + ofToString(maxCaras);
    ofLogNotice() << "alpha: " + ofToString(alpha);
    ofLogNotice() << "watchFolderRefresh: " + ofToString(watchFolderRefresh);
    ofLogNotice() << "aceleracionNormal: " + ofToString(aceleracionNormal);
    ofLogNotice() << "aceleracionBordes: " + ofToString(aceleracionBordes);
}
//--------------------------------------------------------------
void ofApp::watchFolderFileAdded(string& filename)
{
    ofLogNotice("Archivo '" + filename + "' AGREGADO! - TotalCaras:" + ofToString(totalCaras) + " - Max Caras: "+ ofToString(maxCaras) +" - ABSOLUTE MAX CARAS: " + ofToString(ABSOLUTE_MAX_CARAS));

    ofFile newFile(filename);
    if (newFile.getExtension() == "png") {
        if (currentCara == -1) {
            currentCara = 0;
        }
        // Si llegué al máximo, eliminar la primera cara
        if (totalCaras >= maxCaras) {
            eliminarCara(carasFileNames[0]);
        }
        carasFileNames[totalCaras] = filename;
        caras[totalCaras].loadImage(ofToString(CARAS_PATH) + "/" + filename);

        posicionCaras[totalCaras] = ofPoint(ofMap(ofRandomWidth(),0,ofGetWindowWidth(),0,ofGetWindowWidth() - caras[totalCaras].width),
                                            ofMap(ofRandomHeight(),0,ofGetWindowHeight(),0,ofGetWindowHeight() - caras[totalCaras].height));
        velocidadCaras[totalCaras] = ofPoint(ofRandom(1)*maxVelInicial.x,ofRandom(1)*maxVelInicial.y);
        aceleracionCaras[totalCaras] = ofPoint(0.0,0.0);
//ofLogNotice(" [" + ofToString(totalCaras) + "] velocidad x: "+ ofToString(velocidadCaras[totalCaras].x)+ " - y: " + ofToString(velocidadCaras[totalCaras].y));
        totalCaras++;
    }
}
//--------------------------------------------------------------
void ofApp::watchFolderFileRemoved(string& filename)
{
    ofLogNotice("Archivo '" + filename + "' ELIMINADO!");
    eliminarCara(filename);
}
//--------------------------------------------------------------
void ofApp::eliminarCara(string filename)
{
    int index = getFilenameIndex(filename);
    if (index >= 0) {
        // Move everything down
        for(int i = index; i < (totalCaras-1); i++){
            carasFileNames[i] = carasFileNames[i+1];
            caras[i] = caras[i+1];
            posicionCaras[i] = posicionCaras[i+1];
            velocidadCaras[i] = velocidadCaras[i+1];
            aceleracionCaras[i] = aceleracionCaras[i+1];
        }
        totalCaras--;
    }
}
//--------------------------------------------------------------
int ofApp::getFilenameIndex(string filename)
{
    for(int i = 0; i < totalCaras; i++){
        if (carasFileNames[i] == filename) {
            return i;
        }
    }
    return -1;
}
//--------------------------------------------------------------
void ofApp::update(){
/*
    for(int i = 0; i < totalCaras; i++){
        posicionCaras[i].x += velocidadCaras[i].x;
        posicionCaras[i].y += velocidadCaras[i].y;
        if (((posicionCaras[i].x+caras[i].getWidth()) >= ofGetWindowWidth()) || (posicionCaras[i].x<0)) {
            velocidadCaras[i].x = -velocidadCaras[i].x;
        }
        if (((posicionCaras[i].y+caras[i].getHeight()) >= ofGetWindowHeight()) || (posicionCaras[i].y<0)) {
            velocidadCaras[i].y = -velocidadCaras[i].y;
        }
    }
*/

//    viento = ofPoint((float)mouseX-ofGetWindowWidth()/2,(float)mouseY-ofGetWindowHeight()/2);
//    viento.x = viento.x/(ofGetWindowWidth()/2);
//    viento.y = viento.y/(ofGetWindowHeight()/2);
//    viento = viento / 10;

    float factorPulsacion = 0.1;
    viento.x = (sin(ofGetElapsedTimef()*factorPulsacion));
    viento.y = (cos(ofGetElapsedTimef()*factorPulsacion));
    viento = viento / 1e6;

    for(int i = 0; i < totalCaras; i++){
        if (((posicionCaras[i].x+caras[i].getWidth()*2) >= ofGetWindowWidth()) || (posicionCaras[i].x<0+caras[i].getWidth())) {
            aceleracionCaras[i].x = ((posicionCaras[i].x<0+caras[i].getWidth())?aceleracionBordes.x:-aceleracionBordes.x);
        }  else {
            aceleracionCaras[i].x = ((aceleracionCaras[i].x>=0)?aceleracionNormal.x:-aceleracionNormal.x);
        }

        if (((posicionCaras[i].y+caras[i].getHeight()*2) >= ofGetWindowHeight()) || (posicionCaras[i].y<0+caras[i].getHeight())) {
            aceleracionCaras[i].y = ((posicionCaras[i].y<0+caras[i].getHeight())?aceleracionBordes.y:-aceleracionBordes.y);
        } else {
            aceleracionCaras[i].y = ((aceleracionCaras[i].y>=0)?aceleracionNormal.y:-aceleracionNormal.y);
        }

        float margen = 100.0;
        ofPoint wrappedPos = ofPoint(min(max(posicionCaras[i].x,-margen),ofGetWindowWidth()+margen),
                                     min(max(posicionCaras[i].y,-margen),ofGetWindowHeight()+margen));
        if (wrappedPos == posicionCaras[i]) {
            // El viento afecta más fuerte a los elementos más chicos
            // Cabeza grande: superficie 70.000
            // Cabeza chica: superficie 1000
            float superficie = caras[i].width*caras[i].height;
            float dividendo = 10000.0;
            velocidadCaras[i].x += aceleracionCaras[i].x + viento.x*(dividendo/superficie);
            velocidadCaras[i].y += aceleracionCaras[i].y + viento.y*(dividendo/superficie);

            posicionCaras[i].x += velocidadCaras[i].x;
            posicionCaras[i].y += velocidadCaras[i].y;
        } else {
            posicionCaras[i] = wrappedPos;
            if (i==0) {
            //    ofLogNotice("Wrapped!");
            }
        }
        // Limito máx velocidad
        if (abs(velocidadCaras[i].x) > maxVelocidad.x) {
                velocidadCaras[i].x = ofSign(velocidadCaras[i].x)*maxVelocidad.x;
        }
        if (abs(velocidadCaras[i].y) > maxVelocidad.y) {
                velocidadCaras[i].y = ofSign(velocidadCaras[i].y)*maxVelocidad.y;
        }
    }

    /**
        Busco la cara más cercana, y voy hacia ella.
        Pero evito los 'encuentros', es decir, si una cara es la 'cercana' de otra,
        entonces, esa otra no va a elegir a esta como la más cercana.
    */
/*
    int carasCercanasA[ABSOLUTE_MAX_CARAS];
    int caraMasCercana[ABSOLUTE_MAX_CARAS];
    for(int i = 0; i < totalCaras; i++){
        caraMasCercana[i] = -1;
        carasCercanasA[i] = 0;
    }

    const int ABSOLUTE_MAX_CARAS_CERCANAS = 4;
    for(int i = 0; i < totalCaras; i++){
        float minDist = 500.0;
        // Lo inicializo con una cara arbitraria
        caraMasCercana[i] = totalCaras-1-i;
        for(int j = i+1; j < totalCaras; j++){
            float currDist = abs(ofDist(posicionCaras[i].x,posicionCaras[i].y,
                                ofWrap(posicionCaras[j].x,0,ofGetWindowWidth()),
                                ofWrap(posicionCaras[j].y,0,ofGetWindowHeight())));
            if ((currDist!=0) && (currDist < minDist) && (caraMasCercana[j] != i)) {
                if (carasCercanasA[j]<ABSOLUTE_MAX_CARAS_CERCANAS) {
                    minDist = currDist;
                    caraMasCercana[i] = j;
                    //ofLogNotice("Cara cercana a " + ofToString(i) + " : " + ofToString(j) + " - Dist: " + ofToString(currDist));
                }
            }
        }
        carasCercanasA[caraMasCercana[i]]++;
        //ofLogNotice("Cara mas cercana a " + ofToString(i) + " : " + ofToString(caraMasCercana[i]) + " - Dist: " + ofToString(currDist));
        //ofLogNotice("carasCercanasA " + ofToString(caraMasCercana[i]) + " : " + ofToString(carasCercanasA[caraMasCercana[i]]));
    }

    for(int i = 0; i < totalCaras; i++){
        ofPoint pDiff(posicionCaras[caraMasCercana[i]].x - posicionCaras[i].x,posicionCaras[caraMasCercana[i]].y - posicionCaras[i].y);
        aceleracionCaras[i].x += (pDiff.x>0?0.01:(pDiff.x<0?-0.01:0));
        aceleracionCaras[i].y += (pDiff.y>0?0.01:(pDiff.y<0?-0.01:0));

        velocidadCaras[i].x += aceleracionCaras[i].x;
        velocidadCaras[i].y += aceleracionCaras[i].y;

        posicionCaras[i].x += velocidadCaras[i].x;
        posicionCaras[i].y += velocidadCaras[i].y;
    }
*/
/*
    if (currentCara != -1) {
        currentCara++;
    }
    if (currentCara >= totalCaras) {
        currentCara = 0;
    }
*/
}

//--------------------------------------------------------------
void ofApp::draw(){

    /*
    ofColor colorOne;
    ofColor colorTwo;

    colorOne.set (0, 0, 127);
    colorTwo.set (0, 0, 0);
    ofBackgroundGradient(colorOne, colorTwo, OF_GRADIENT_LINEAR);
    */
      // Sets the background to a bar gradient

    // Mostrar Vector de Viento
    if (debug) {
        ofPoint startPoint, endPoint;
        startPoint = ofPoint(ofGetWindowWidth()/2,ofGetWindowHeight()/2);
        endPoint = (startPoint + (viento*1e8));

        ofSetColor(255,255,255,255);
        ofLine(startPoint,endPoint);

        ofDrawBitmapString(ofToString(viento.x)+":"+ofToString(viento.y),10,20);
        ofDrawBitmapString(ofToString(startPoint.x)+":"+ofToString(startPoint.y),10,40);
        ofDrawBitmapString(ofToString((int)endPoint.x)+":"+ofToString((int)endPoint.y),10,60);
    }


/*
 glMatrixMode(GL_TEXTURE);
    glPushMatrix();
    ofScale(2048,1024);
    glMatrixMode(GL_MODELVIEW);


    ofPushMatrix();


    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);


    //easyCam.begin();  // If uncommented, weird texture inversion

    ofRotateX(-30);
    ofRotateY(-60);

*/
    for(int i = 0; i < totalCaras; i++){
        //int alpha = ((float(i)+1.0)/totalCaras)*255;
        //int alpha = 127;
        ofSetColor(255,255,255,alpha);
        //ofLogNotice("i = " + ofToString(i) + " - ratio: " + ofToString((float(i)/(totalCaras-1))) + " - alpha = " + ofToString(alpha));
        caras[i].draw(posicionCaras[i].x,posicionCaras[i].y);
        if (debug) {
            ofSetColor(255,0,0,255);
            //ofDrawBitmapString(ofToString(caras[i].width*caras[i].height),posicionCaras[i].x,posicionCaras[i].y);
            ofDrawBitmapString("Acel: " + ofToString(aceleracionCaras[i].x)+":"+ofToString(aceleracionCaras[i].y),posicionCaras[i].x,posicionCaras[i].y);
            ofDrawBitmapString("Vel: " + ofToString(velocidadCaras[i].x)+":"+ofToString(velocidadCaras[i].y),posicionCaras[i].x,posicionCaras[i].y+20);
        }
        /*
        float radio = 400.0;
        caras[i].getTextureReference().bind();
        ofDrawSphere(posicionCaras[i],radio);
        caras[i].getTextureReference().unbind();
        */
    }

/*
    //easyCam.end(); // If uncommented, weird texture inversion

    ofPopMatrix();


    glMatrixMode(GL_TEXTURE);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
*/
    /*
    if (currentCara != -1) {
        //currentCara = 0;
        ofLogNotice("currentCara = " + ofToString(currentCara));
        caras[currentCara].draw((ofGetWindowWidth()-caras[currentCara].width)/2,
                                (ofGetWindowHeight()-caras[currentCara].height)/2);
    }
    */
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
