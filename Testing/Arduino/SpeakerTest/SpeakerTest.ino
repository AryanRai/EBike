#include "SoundData.h"
#include "XT_DAC_Audio.h"
XT_Wav_Class Sound_activate(activate); 
//XT_Wav_Class Sound_deactivate(deactivate); 
//XT_Wav_Class Sound_seatbelt(seatbelt); 
//XT_Wav_Class Sound_tripple_alert(tripple_alert); 
//XT_Wav_Class Sound_voice(voice);                                          
XT_DAC_Audio_Class DacAudio(25,0);                 
void setup() {
  Serial.begin(115200);               
}
void loop() {
  DacAudio.FillBuffer();                
  //if(Sound.Playing==false)       
    //DacAudio.Play(&Sound);       
 //Serial.println(DemoCounter++);     
  //if (Serial.readString() == "1") {
    DacAudio.Play(&Sound_activate);
    Serial.println("play 1");
    delay(10000);
  //}

  //if (Serial.readString() == "2") {
    //DacAudio.Play(&Sound_deactivate);
    //Serial.println("play 2");
  //}
   
  //if (Serial.readString() == "3") {
    //DacAudio.Play(&Sound_tripple_alert);
    //Serial.println("play 3");
  //}
     
}
