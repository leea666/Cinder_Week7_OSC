/* By Aaron and Danli
Two Player Game
This code is for the host/listner 
recieving the messages from the
other computer
 */

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "OscListener.h"
#include "OscSender.h"
#include "cinder/Rand.h"



using namespace ci;
using namespace ci::app;
using namespace std;

class Week7_HostApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
    void mouseUp( MouseEvent event) override ;
	void update() override;
	void draw() override;
    float ballPosition() ;
    void resetTarget() ;
    
    osc::Listener 	listener;
    osc::Sender sender;
    std::string host;
    int 		port;
    
    
    float 			posXBall1 ;
    float           posYBall1 ;
    float 			posXBall2 ;
    float           posYBall2 ;
    float           mSpeedBall1 ;
    float           mSpeedBall2 ;
    float           mAngleBall1 ;
    float           mAngleBall2 ;
    float           mRadiusBall1 ;
    float           mRadiusBall2 ;
    float           mFrameRate ;
    bool            mouseHolder = false ;
    float           posXTarget ;
    float           posYTarget ;
    int             ball1Score ;
    int             ball2Score ;
    
    bool            mTimeOut() ;
    int             mTimeCount ;
};

void Week7_HostApp::setup()
{
    listener.setup( 3000 );
    posXBall1 = 0.f ;
    posYBall1 = 0.f ;
    mSpeedBall1 = 0.02f ;
    mSpeedBall2 = 0.02f ;
    mAngleBall1 = 0.f ;
    mAngleBall2 = 180.f ;
    mRadiusBall1 = 200.f ;
    mRadiusBall2 = 200.f ;
    mFrameRate = getFrameRate() ;
    resetTarget() ;
    ball1Score = 0 ;
    ball2Score = 0 ;
    
    host = "149.31.143.91" ;
    port = 5000 ;
    sender.setup( host, port, true );
}

void Week7_HostApp::mouseDown( MouseEvent event )
{
    mouseHolder = true ;
//    mSpeedBall2 = mSpeedBall2 + 0.1f ;
//    mSpeedBall1 = mSpeedBall1 + 0.1f ;
//    mRadiusBall1 = abs(mRadiusBall1 - 1.f) ;
//    mRadiusBall2 = abs(mRadiusBall2 - 1.f) ;
}

void Week7_HostApp::mouseUp( MouseEvent event )
{
    mouseHolder = false ;
    
}

void Week7_HostApp::update()
{
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
//                    console() << "------ value: " << message.getArgAsFloat(i) << std::endl;
                }
                catch (...) {
//                    console() << "Exception reading argument as float" << std::endl;
                }
            }
            else if( message.getArgType(i) == osc::TYPE_STRING) {
                try {
//                    console() << "------ value: " << message.getArgAsString(i).c_str() << std::endl;
                }
                catch (...) {
//                    console() << "Exception reading argument as string" << std::endl;
                }
            }
        }
        
        if( message.getNumArgs() != 0 && message.getArgType( 0 ) == osc::TYPE_FLOAT ) {
            posXBall1 = message.getArgAsFloat(0);
            posYBall1 = message.getArgAsFloat(1) ;
            mTimeCount = message.getArgAsFloat(2) ;
//            console() << posXBall1 << std::endl ;
        }
    }
    
    osc::Message message;
    message.setAddress("posX");
    message.addFloatArg(posXBall2);
    message.setAddress("posY");
    message.addFloatArg(posYBall2);
    message.setAddress("posX of Target");
    message.addFloatArg(posXTarget);
    message.setAddress("posY of Target");
    message.addFloatArg(posYTarget);
    message.setAddress("Red Score");
    message.addFloatArg(ball1Score);
    message.setAddress("Blue Score");
    message.addFloatArg(ball2Score);
    sender.sendMessage(message);

    
   
    if(mouseHolder == true) {
        ballPosition() ;
        if(mSpeedBall2 < 0.5f) {
            mSpeedBall2 = mSpeedBall2 + 0.05f ;
        } else {
            mSpeedBall2 = 0.5f ;
        }
        mRadiusBall2 = abs(mRadiusBall2 - 5.f) ;
    } else {
        ballPosition() ;
        if(mSpeedBall2 > 0.02f) {
            mSpeedBall2 = mSpeedBall2 - 0.01f ;
        } else {
            mSpeedBall2 = 0.02f ;
        }
        if(mRadiusBall2 < 200.f) {
            mRadiusBall2 = mRadiusBall2 + 2.f ;
        }
    }
    
    float closeDist = 20 ;
    float distBall2 = sqrt(((posXBall2 - posXTarget)*(posXBall2 - posXTarget)) + ((posYBall2 - posYTarget) * (posYBall2 - posYTarget))) ;
    float distBall1 = sqrt(((posXBall1 - posXTarget)*(posXBall1 - posXTarget)) + ((posYBall1 - posYTarget) * (posYBall1 - posYTarget))) ;
    
    if(distBall1 < closeDist) {
        resetTarget() ;
        ball1Score++ ;
    }
    
    if(distBall2 < closeDist) {
        resetTarget() ;
        ball2Score++ ;
    }
    


}

float Week7_HostApp::ballPosition() {

    mAngleBall2 += mSpeedBall2 ;
    if(mAngleBall2 >= 360) {
        mAngleBall2 = 0 ;
    }
    posXBall2 = cos(mAngleBall2) * mRadiusBall2 ;
    console() << posXBall2 << std::endl ;

    posYBall2 = sin(mAngleBall2) * mRadiusBall2 ;
    console() << posYBall2<< std::endl ;
    return 0.1f ;
    
}

void Week7_HostApp::draw()
{
    if (!mTimeOut()) {
	gl::clear( Color( 0, 0, 0 ) );
    gl::enableAlphaBlending() ;
    gl::pushModelMatrix() ;
    gl::translate(getWindowCenter()) ;
    
    {
        gl::ScopedColor ball1(1.0,0,0) ;
        gl::drawSolidCircle(glm::vec2(posXBall1,posYBall1), 10) ;
        gl::drawLine(glm::vec2(0,0), glm::vec2(posXBall1, posYBall1)) ;
    }
    
    {
        gl::ScopedColor ball2(0,0,1.0) ;
        gl::drawSolidCircle(glm::vec2(posXBall2, posYBall2), 10) ;
        gl::drawLine(glm::vec2(0,0), glm::vec2(posXBall2, posYBall2)) ;
    }
    

    {
        gl::ScopedColor targetColor(0, 1.0, 0) ;
        gl::drawSolidCircle(glm::vec2(posXTarget, posYTarget), 10) ;
    }
    
    gl::popModelMatrix() ;
    
    gl::drawString( "Red Player Score: " + std::to_string(ball1Score) , ci::vec2( 20.f, 20.f ), ci::Color( 0.5f, 0.5f, 0.5f ) );
    gl::drawString( "Blue Player Score: " + std::to_string(ball2Score) , ci::vec2( 20.f, 35.f ), ci::Color( 0.5f, 0.5f, 0.5f ) );
        
    int timeLeft = 1000 - mTimeCount;
    int timeLength = lmap(timeLeft, 0, 1000, 0, getWindowWidth());
    gl::color(0.f, 1.f, 0.f);
    gl::drawSolidRect(Rectf(0.f,getWindowHeight()-10,timeLength,getWindowHeight()));

    }
    
    if (mTimeOut()) {
        if (ball1Score > ball2Score) {
            gl::drawString("RED WIN!", vec2(200.f,200.f), ci::Color(1.f,0.f,0.f));
        }else if(ball2Score > ball1Score) {
            gl::drawString("BlUE WIN!", vec2(200.f,200.f), ci::Color(0.f,0.f,1.f));
        }else{
            gl::drawString("DRAW", vec2(200.f,200.f), ci::Color(0.f,1.f,0.f));
        }
    }
    
}

void Week7_HostApp::resetTarget() {
    posXTarget = ci::randFloat(-150, 150) ;
//    console() << posXTarget << std::endl ;
    posYTarget = ci::randFloat(-150, 150) ;
//    console() << posYTarget << std::endl ;
}

bool Week7_HostApp::mTimeOut(){
    
    if (1000-mTimeCount <= 0) {
        return true;
    }else{
        return false;
    }
}

CINDER_APP( Week7_HostApp, RendererGl )
