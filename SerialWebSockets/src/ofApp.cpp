#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetLogLevel(OF_LOG_VERBOSE);

	// setup a server with default options on port 9092
	// - pass in true after port to set up with SSL
	//bSetup = server.setup( 9093 );

	//Setup WebSockets
	ofxLibwebsockets::ServerOptions options = ofxLibwebsockets::defaultServerOptions();
	options.port = PORT_WEB;
	options.bUseSSL = false; // you'll have to manually accept this self-signed cert if 'true'!
	bSetup = server.setup(options);

	// this adds your app as a listener for the server
	server.addListener(this);

	ofBackground(0);
	ofSetFrameRate(60);

	ofFile file("test_web.json");
	if (file.exists()) {
		file >> js;
		toSend = js.dump();
	}
	cout << "Port" << PORT_WEB << std::endl;
	cout << "JSON read:" << std::endl;
	cout << toSend << std::endl;

	//Serial communication

	serial.listDevices();
	vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
	for (auto & list : deviceList) {
		cout << list.getDeviceName() << std::endl;
	}
	int baud = 9600;
	cout << "Loading " << deviceList.back().getDevicePath() << std::endl;
	bool loadSerial = serial.setup(deviceList.back().getDevicePath(), baud);
	if (loadSerial == false) {
		cout << "Error Loading Serial Port " << std::endl;
	}
	else {
		cout << "Loaded " << deviceList.back().getDevicePath() << std::endl;
	}

	buttonRect = ofRectangle(400, 50, 50, 50);
}

//--------------------------------------------------------------
void ofApp::update() {

	if (serial.available()> 0) {

		uint8_t serialRead[4];
		memset(serialRead, 0, 4);

		// read from the serial port  
		serial.readBytes(serialRead, 4);

		memcpy(serialInput, serialRead, 4);

		cout << "got msg serial" << std::endl;
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	if (bSetup) {
		ofDrawBitmapString("WebSocket server setup at " + ofToString(server.getPort()) + (server.usingSSL() ? " with SSL" : " without SSL"), 20, 20);

		ofSetColor(150);
		ofDrawBitmapString("Click anywhere to open up client example", 20, 40);
	}
	else {
		ofDrawBitmapString("WebSocket setup failed :(", 20, 20);
	}

	ofPushStyle();
	for (int i = 0; i < 4; i++) {
		float rx = 550 + i*70;
		float ry = 100;
		float rw = 50;
		float rh = 50;
		
		if (serialInput[i] == 0) {
			ofSetColor(0, 150, 200);
		}
		else {
			ofSetColor(50);
		}
		ofDrawRectangle(rx, ry, rw, rh);
	}
	ofPopStyle();

	//button connect
	ofSetColor(0, 200, 150, 150);
	ofDrawRectangle(buttonRect);

	int x = 20;
	int y = 100;

	ofSetColor(0, 150, 0);
	ofDrawBitmapString("Console", x, 80);

	ofSetColor(255);
	for (int i = messages.size() - 1; i >= 0; i--) {
		ofDrawBitmapString(messages[i], x, y);
		y += 20;
	}
	if (messages.size() > NUM_MESSAGES) messages.erase(messages.begin());

	ofSetColor(150, 0, 0);
	ofDrawBitmapString("Type a message, hit [RETURN] to send:", x, ofGetHeight() - 60);
	ofSetColor(255);
	ofDrawBitmapString(toSend, x, ofGetHeight() - 40);
}

//--------------------------------------------------------------
void ofApp::onConnect(ofxLibwebsockets::Event& args) {
	cout << "on connected" << endl;
}

//--------------------------------------------------------------
void ofApp::onOpen(ofxLibwebsockets::Event& args) {
	cout << "new connection open" << endl;
	messages.push_back("New connection from " + args.conn.getClientIP() + ", " + args.conn.getClientName());
}

//--------------------------------------------------------------
void ofApp::onClose(ofxLibwebsockets::Event& args) {
	cout << "on close" << endl;
	messages.push_back("Connection closed");
}

//--------------------------------------------------------------
void ofApp::onIdle(ofxLibwebsockets::Event& args) {
	cout << "on idle" << endl;
}

//--------------------------------------------------------------
void ofApp::onMessage(ofxLibwebsockets::Event& args) {
	cout << "got message " << args.message << endl;

	// trace out string messages or JSON messages!
	if (!args.json.is_null()) {
		messages.push_back("New message: " + args.json.dump(4) + " from " + args.conn.getClientName());
	}
	else {
		messages.push_back("New message: " + args.message + " from " + args.conn.getClientName());
	}



	// echo server = send message right back!
	args.conn.send(toSend);
}

//--------------------------------------------------------------
void ofApp::onBroadcast(ofxLibwebsockets::Event& args) {
	cout << "got broadcast " << args.message << endl;
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// do some typing!
	if (key != OF_KEY_RETURN) {
		if (key == OF_KEY_BACKSPACE) {
			if (toSend.length() > 0) {
				toSend.erase(toSend.end() - 1);
			}
		}
		else {
			toSend += key;
		}
	}
	else {
		// send to all clients
		server.send(toSend);
		messages.push_back("Sent: '" + toSend + "' to " + ofToString(server.getConnections().size()) + " websockets");
		toSend = "";
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (buttonRect.inside( ofVec2f(x, y))) {
		string url = "http";
		if (server.usingSSL()) {
			url += "s";
		}
		url += "://localhost:" + ofToString(server.getPort());
		ofLaunchBrowser(url);
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
