#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/System.h"

#include "OscSender.h"
#include "OscListener.h"

#include "cinder/Rand.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class w7_twoPlayerGame_SenderApp : public App {
    
public:
    void setup() override;
    void mouseDown( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void update() override;
    void draw() override;
    void resetTarget();
    
//    bool timeOut();
//    int timeCount;
    
    osc::Sender sender;
    osc::Listener 	listener;
    
    //    std::string host;
    int SenderPort, ListenerPort;
    float radius;
    float speed;
    float angle;
    
    float BlueX, BlueY;
    float targetX, targetY;
    
    vec2 pos;
    vec2 pos2;
    vec2 targetPos;
    
    int redScore, blueScore;
    bool mouseHold;
    
};

void w7_twoPlayerGame_SenderApp::setup()
{
    mouseHold = false;
    
    redScore = 0;
    blueScore = 0;
    
    angle = 0.f;
    SenderPort = 3000;
    ListenerPort = 5000;
    speed = 0.02f;
    radius = 200.f;
    
    sender.setup("149.31.125.171", SenderPort, true);
    listener.setup(5000);
    
    BlueX = 0;
    BlueY = 0;
    targetX = 0;
    targetY = 0;
    targetPos = vec2(0, 0);
    pos2 = vec2(BlueX, BlueY);
    
//    timeCount = 0;
}

void w7_twoPlayerGame_SenderApp::mouseDown( MouseEvent event )
{
    mouseHold = true;
}

void w7_twoPlayerGame_SenderApp::mouseUp( MouseEvent event )
{
    mouseHold = false;
}

void w7_twoPlayerGame_SenderApp::update()
{
    //Sender
    osc::Message message;
    message.setAddress("Red x");
    message.addFloatArg(pos.x);
    message.setAddress("Red y");
    message.addFloatArg(pos.y);
//    message.setAddress("Timeout");
//    message.addIntArg(timeCount);
    sender.sendMessage(message);
    
    //Listener
    while( listener.hasWaitingMessages() ) {
        osc::Message message;
        listener.getNextMessage( &message );
        
        console() << "New message received" << std::endl;
        console() << "Address: " << message.getAddress() << std::endl;
        console() << "Num Arg: " << message.getNumArgs() << std::endl;
        for (int i = 0; i < message.getNumArgs(); i++) {
            console() << "-- Argument " << i << std::endl;
            console() << "---- type: " << message.getArgTypeName(i) << std::endl;
            if( message.getArgType(i) == osc::TYPE_INT32 ) {
                try {
                    console() << "------ value: "<< message.getArgAsInt32(i) << std::endl;
                }
                catch (...) {
                    console() << "Exception reading argument as int32" << std::endl;
                }
            }
            else if( message.getArgType(i) == osc::TYPE_FLOAT ) {
                try {
                    console() << "------ value: " << message.getArgAsFloat(i) << std::endl;
                }
                catch (...) {
                    console() << "Exception reading argument as float" << std::endl;
                }
            }
            else if( message.getArgType(i) == osc::TYPE_STRING) {
                try {
                    console() << "------ value: " << message.getArgAsString(i).c_str() << std::endl;
                }
                catch (...) {
                    console() << "Exception reading argument as string" << std::endl;
                }
            }
        }
        
        if( message.getNumArgs() != 0 && message.getArgType( 0 ) == osc::TYPE_FLOAT ){
            BlueX = message.getArgAsFloat(0);
            BlueY = message.getArgAsFloat(1);
            targetX = message.getArgAsFloat(2);
            targetY = message.getArgAsFloat(3);
            redScore = message.getArgAsFloat(4);
            blueScore = message.getArgAsFloat(5);
        }
        
        pos2 = vec2(BlueX, BlueY);
        targetPos = vec2(targetX, targetY);
    }
    
    angle += speed;
    
    if (mouseHold) {
        speed += 0.1f;
        radius -= 5.f;
    }
    
    if (!mouseHold) {
        speed -= 0.02f;
        radius += 2.f;
    }
    
    if (speed < 0.02f) {
        speed = 0.02f;
    }
    
    if (speed > 0.5f) {
        speed = 0.5f;
    }
    
    if (radius > 200.f) {
        radius = 200.f;
    }
    
    if (radius < 10.f) {
        radius = 10.f;
    }
    
    if(radius > 360.f){
        radius = 0;
    }
    
    cout << "speed" << speed << "radius" << radius << endl;
    
    float x = radius * cos(angle);
    float y = radius * sin(angle);
    pos = vec2(x, y);
    
    //    float dist = sqrt(((pos.x-targetPos.x)*(pos.x-targetPos.x))+((pos.y-targetPos.y)*(pos.y-targetPos.y)));
    //
    //    float someRange = 20.f;
    //    if (dist < someRange) {
    //        redScore ++;
    //        resetTarget();
    //    }
    
//    timeCount ++;
}

void w7_twoPlayerGame_SenderApp::draw()
{
//    if (!timeOut()) {
    
    gl::clear( Color( 0, 0, 0 ) );
    gl::pushMatrices();
    gl::translate(getWindowCenter());
    
    //Red
    gl::color(1.f, 0.f, 0.f);
    gl::drawSolidCircle(pos, 10);
    gl::drawLine(vec2(0,0), pos);
    
    //Blue
    gl::color(0.f, 0.f, 1.f);
    gl::drawSolidCircle(pos2, 10);
    gl::drawLine(vec2(0,0), pos2);
    
    //target
    gl::color(0.f, 1.f, 0.f);
    gl::drawSolidCircle(targetPos, 10);
    
    gl::popMatrices();
    
    //Score
    gl::drawString("Red Player Score: " + std::to_string(redScore), vec2(20.f,20.f), ci::Color(0.5f,0.5f,0.5f));
    gl::drawString("Blue Player Score: " + std::to_string(blueScore), vec2(20.f,35.f), ci::Color(0.5f,0.5f,0.5f));
    
    //Time Out
//    int timeLeft = 1000 - timeCount;
//    int timeLength = lmap(timeLeft, 0, 1000, 0, getWindowWidth());
//    gl::color(0.f, 1.f, 0.f);
//    gl::drawSolidRect(Rectf(0.f,getWindowHeight()-10,timeLength,getWindowHeight()));
    
//    }

//    if (timeOut()) {
//        if (redScore > blueScore) {
//            gl::drawString("RED WIN!", vec2(200.f,200.f), ci::Color(1.f,0.f,0.f));
//        }else if(blueScore > redScore) {
//            gl::drawString("BlUE WIN!", vec2(200.f,200.f), ci::Color(0.f,0.f,1.f));
//        }else{
//            gl::drawString("DRAW", vec2(200.f,200.f), ci::Color(0.f,1.f,0.f));
//        }
//    }
}

//bool w7_twoPlayerGame_SenderApp::timeOut(){
//    
//    if (1000-timeCount <= 0) {
//        return true;
//    }else{
//        return false;
//    }
//}

//void w7_twoPlayerGame_SenderApp::resetTarget()
//{
//    float someNum = 100;
//    targetPos = vec2(randFloat(-someNum,someNum),randFloat(-someNum, someNum));
//    cout << "reset Target" << endl;
//}

CINDER_APP( w7_twoPlayerGame_SenderApp, RendererGl )
