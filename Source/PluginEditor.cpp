#include "PluginEditor.h"
namespace {
const auto bg=juce::Colour(0xff090d14), panel=juce::Colour(0xff151f2c), stroke=juce::Colour(0xff303b4a), accent=juce::Colour(0xffe6b77a), muted=juce::Colour(0xff8795a8), white=juce::Colour(0xfff1f5fa);
void sectionText(juce::Graphics& g, const juce::String& text, juce::Rectangle<int> area) {
 g.setColour(muted); g.setFont(juce::Font(juce::FontOptions(12.f,juce::Font::bold))); g.drawText(text,area,juce::Justification::centredLeft);
}
void panelBox(juce::Graphics& g, juce::Rectangle<float> r) {
 g.setColour(panel);g.fillRoundedRectangle(r,13.f);g.setColour(stroke);g.drawRoundedRectangle(r,13.f,1.f);
}
}
void BassforgeEditor::ForgeKeyboard::drawWhiteNote(int note, juce::Graphics& g, juce::Rectangle<float> area, bool down, bool over, juce::Colour, juce::Colour) {
 g.setColour(down?accent:(over?juce::Colour(0xffd7e4e8):juce::Colour(0xffc1cbd5)));
 g.fillRoundedRectangle(area.reduced(1.f,1.f),3.f);
 g.setColour(juce::Colour(0xff657383));g.drawRoundedRectangle(area.reduced(1.f,1.f),3.f,0.7f);
 if(note%12==0){g.setColour(juce::Colour(0xff405060));g.setFont(juce::Font(juce::FontOptions(10.f,juce::Font::bold)));g.drawText(juce::MidiMessage::getMidiNoteName(note,true,true,3),area.removeFromBottom(17.f),juce::Justification::centred);}
}
void BassforgeEditor::ForgeKeyboard::drawBlackNote(int, juce::Graphics& g, juce::Rectangle<float> area, bool down, bool over, juce::Colour) {
 g.setColour(down?accent:(over?juce::Colour(0xff43566a):juce::Colour(0xff202b39)));
 g.fillRoundedRectangle(area.reduced(.5f,0.f),3.f);g.setColour(juce::Colour(0xff070b10));g.drawRoundedRectangle(area.reduced(.5f,0.f),3.f,1.f);
}
BassforgeEditor::ForgeLookAndFeel::ForgeLookAndFeel(){
 setColour(juce::ComboBox::backgroundColourId,juce::Colour(0xff1b2a38));
 setColour(juce::ComboBox::outlineColourId,stroke);
 setColour(juce::ComboBox::textColourId,juce::Colours::white);
 setColour(juce::PopupMenu::backgroundColourId,panel);
 setColour(juce::PopupMenu::textColourId,juce::Colours::white);
 setColour(juce::TextButton::textColourOffId,juce::Colours::white);
 setColour(juce::Slider::textBoxTextColourId,muted);
 setColour(juce::Slider::textBoxOutlineColourId,juce::Colours::transparentBlack);
 setColour(juce::Slider::textBoxBackgroundColourId,juce::Colours::transparentBlack);
 setColour(juce::ToggleButton::textColourId,muted);
}
void BassforgeEditor::ForgeLookAndFeel::drawRotarySlider(juce::Graphics& g,int x,int y,int width,int height,float pos,float start,float end,juce::Slider&){
 const float cx=x+width*.5f,cy=y+height*.43f,r=juce::jmin(width,height)*.275f;
 g.setColour(juce::Colour(0xff090e17));g.fillEllipse(cx-r-6,cy-r-6,(r+6)*2,(r+6)*2);
 juce::Path ring;ring.addCentredArc(cx,cy,r+5,r+5,0,start,end,true);
 g.setColour(juce::Colour(0xff354153));g.strokePath(ring,juce::PathStrokeType(4.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
 juce::Path value;value.addCentredArc(cx,cy,r+5,r+5,0,start,start+pos*(end-start),true);
 g.setColour(juce::Colour(0xffe6b77a));g.strokePath(value,juce::PathStrokeType(4.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));
 juce::ColourGradient metal(juce::Colour(0xff4b5b6b),cx-r,cy-r,juce::Colour(0xff151d2a),cx+r,cy+r,false);
 g.setGradientFill(metal);g.fillEllipse(cx-r,cy-r,2*r,2*r);
 g.setColour(juce::Colour(0xff758495));g.drawEllipse(cx-r,cy-r,2*r,2*r,1.f);
 float a=start+pos*(end-start);g.setColour(juce::Colour(0xffffe2b8));g.drawLine(cx+std::sin(a)*r*.34f,cy-std::cos(a)*r*.34f,cx+std::sin(a)*r*.8f,cy-std::cos(a)*r*.8f,3.f);
}
void BassforgeEditor::ForgeLookAndFeel::drawButtonBackground(juce::Graphics& g,juce::Button& b,const juce::Colour&,bool over,bool down){
 auto r=b.getLocalBounds().toFloat().reduced(.5f);auto selected=b.getToggleState();
 g.setColour(selected?juce::Colour(0xff17453f):(down?juce::Colour(0xff2a4c58):over?juce::Colour(0xff253949):juce::Colour(0xff1a2937)));
 g.fillRoundedRectangle(r,7.f);g.setColour(selected?accent:stroke);g.drawRoundedRectangle(r,7.f,1.f);
}

BassforgeEditor::BassforgeEditor(BassforgeProcessor& p):AudioProcessorEditor(&p),processor(p){
 setLookAndFeel(&theme);
 setSize(1220,800);
 setResizable(true,true);setResizeLimits(1080,740,1700,1100);
 synthTab.onClick=[this]{showFx=false;updatePage();};fxTab.onClick=[this]{showFx=true;updatePage();};addAndMakeVisible(synthTab);addAndMakeVisible(fxTab);
 title.setText("BASSFORGE",juce::dontSendNotification);title.setFont(juce::Font(juce::FontOptions(29.f,juce::Font::bold)));title.setColour(juce::Label::textColourId,juce::Colours::white);addAndMakeVisible(title);
 subtitle.setText("NEURO SYNTHESIZER   •   PRESET LAB",juce::dontSendNotification);subtitle.setColour(juce::Label::textColourId,muted);addAndMakeVisible(subtitle);
 for(int i=0;i<30;++i)presets.addItem(factoryPatches[(size_t)i].name,i+1);
 presets.setSelectedId(p.getCurrentProgram()+1,juce::dontSendNotification);
 presets.onChange=[this]{processor.setCurrentProgram(presets.getSelectedId()-1);};addAndMakeVisible(presets);
 addAndMakeVisible(savePreset);addAndMakeVisible(loadPreset);addAndMakeVisible(loadPack);
 savePreset.onClick=[this]{
  chooser=std::make_unique<juce::FileChooser>("Save BASSFORGE preset",juce::File::getSpecialLocation(juce::File::userDocumentsDirectory).getChildFile("My Bass.bfpreset"),"*.bfpreset");
  chooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles | juce::FileBrowserComponent::warnAboutOverwriting,[this](const juce::FileChooser& c){auto f=c.getResult();if(f==juce::File{})return;f=f.withFileExtension("bfpreset");auto json=juce::JSON::toString(processor.exportPreset(f.getFileNameWithoutExtension()),true);if(!f.replaceWithText(json))juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,"Save failed",f.getFullPathName());});
 };
 loadPreset.onClick=[this]{
  chooser=std::make_unique<juce::FileChooser>("Load BASSFORGE preset",juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),"*.bfpreset");
  chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,[this](const juce::FileChooser& c){auto f=c.getResult();if(f!=juce::File{})loadPresetFile(f);});
 };
 loadPack.onClick=[this]{
  chooser=std::make_unique<juce::FileChooser>("Load BASSFORGE pack ZIP",juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),"*.zip");
  chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,[this](const juce::FileChooser& c){
   auto f=c.getResult();if(f==juce::File{})return;
   juce::ZipFile zip(f);juce::StringArray names;
   for(int i=0;i<zip.getNumEntries();++i)if(auto* entry=zip.getEntry(i))if(entry->filename.endsWithIgnoreCase(".bfpreset") && !entry->filename.contains("..") && entry->uncompressedSize<1000000)names.add(entry->filename);
   if(names.isEmpty()){juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,"Empty pack","No valid .bfpreset files found.");return;}
   juce::PopupMenu menu;for(int i=0;i<names.size();++i)menu.addItem(i+1,names[i]);
   menu.showMenuAsync(juce::PopupMenu::Options(),[this,f,names](int selection){if(selection<=0)return;juce::ZipFile archive(f);auto stream=std::unique_ptr<juce::InputStream>(archive.createStreamForEntry(archive.getIndexOfFileName(names[selection-1])));if(!stream)return;auto json=juce::JSON::parse(stream->readEntireStreamAsString());juce::String error;if(!processor.importPreset(json,error)){juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,"Preset error",error);return;}refreshControls();});
  });
 };
 const char* ids[]={"drive","fm","detune","cutoff","movement","release","tone","width","sub","output","punch","rasp"};
 for(int i=0;i<12;++i){auto& s=knobs[(size_t)i];s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);s.setTextBoxStyle(juce::Slider::TextBoxBelow,false,65,21);s.setColour(juce::Slider::rotarySliderFillColourId,accent);s.setColour(juce::Slider::thumbColourId,juce::Colours::white);addAndMakeVisible(s);auto& l=labels[(size_t)i];l.setText(juce::String(ids[i]).toUpperCase(),juce::dontSendNotification);l.setJustificationType(juce::Justification::centred);l.setColour(juce::Label::textColourId,juce::Colours::white);addAndMakeVisible(l);attachments[(size_t)i]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.params,ids[i],s);}
 shape.addItemList({"Saw","Horn","Metal"},1);addAndMakeVisible(shape);shapeAttachment=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.params,"shape",shape);
 keyboard.setAvailableRange(24,84);keyboard.setKeyWidth(25.f);keyboard.setScrollButtonsVisible(false);keyboard.setVelocity(0.85f,false);addAndMakeVisible(keyboard);
 bypassAttach=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.params,"fxbypass",bypass);addAndMakeVisible(bypass);
 for(int i=0;i<7;++i){
  auto id="fx"+juce::String(i);
  addAndMakeVisible(fxLabels[(size_t)i]);addAndMakeVisible(fxEnabled[(size_t)i]);
  addAndMakeVisible(fxMix[(size_t)i]);addAndMakeVisible(fxAmount[(size_t)i]);
  auto setup=[](juce::Slider& sl){sl.setSliderStyle(juce::Slider::LinearHorizontal);sl.setTextBoxStyle(juce::Slider::TextBoxRight,false,55,20);};
  setup(fxMix[(size_t)i]);setup(fxAmount[(size_t)i]);
  fxEnabledAttach[(size_t)i]=std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.params,id+"on",fxEnabled[(size_t)i]);
  fxMixAttach[(size_t)i]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.params,id+"mix",fxMix[(size_t)i]);
  fxAmountAttach[(size_t)i]=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.params,id+"amt",fxAmount[(size_t)i]);
  fxUp[(size_t)i].setButtonText("UP");fxDown[(size_t)i].setButtonText("DOWN");
  fxUp[(size_t)i].onClick=[this,i]{for(int j=0;j<7;++j)if(processor.getFxAt(j)==i){processor.moveFx(j,-1);break;}refreshFxLabels();};
  fxDown[(size_t)i].onClick=[this,i]{for(int j=0;j<7;++j)if(processor.getFxAt(j)==i){processor.moveFx(j,1);break;}refreshFxLabels();};
  fxMix[(size_t)i].setColour(juce::Slider::trackColourId,accent);fxAmount[(size_t)i].setColour(juce::Slider::trackColourId,accent);
  addAndMakeVisible(fxUp[(size_t)i]);addAndMakeVisible(fxDown[(size_t)i]);
 }
 refreshFxLabels();updatePage();
}
void BassforgeEditor::loadPresetFile(const juce::File& file){
 if(file.getSize()>1000000){juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,"Invalid preset","File too large.");return;}
 auto data=juce::JSON::parse(file.loadFileAsString());juce::String error;
 if(!processor.importPreset(data,error)){juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,"Preset error",error);return;}
 refreshControls();
}
void BassforgeEditor::refreshControls(){
 presets.setSelectedId(0,juce::dontSendNotification);
 refreshFxLabels();
}
void BassforgeEditor::refreshFxLabels(){
 for(int i=0;i<7;++i){int fx=i;fxLabels[(size_t)i].setText(juce::String([this,i]{for(int j=0;j<7;++j)if(processor.getFxAt(j)==i)return j+1;return i+1;}())+"  "+BassforgeProcessor::fxNames[fx],juce::dontSendNotification);}
}
void BassforgeEditor::updatePage(){
 for(auto& x:knobs)x.setVisible(!showFx);for(auto& x:labels)x.setVisible(!showFx);shape.setVisible(!showFx);
 for(int i=0;i<7;++i){fxLabels[(size_t)i].setVisible(showFx);fxEnabled[(size_t)i].setVisible(showFx);fxMix[(size_t)i].setVisible(showFx);fxAmount[(size_t)i].setVisible(showFx);fxUp[(size_t)i].setVisible(showFx);fxDown[(size_t)i].setVisible(showFx);}
 bypass.setVisible(showFx);shape.setVisible(!showFx);synthTab.setToggleState(!showFx,juce::dontSendNotification);fxTab.setToggleState(showFx,juce::dontSendNotification);repaint();
}
void BassforgeEditor::paint(juce::Graphics& g){
 const float w=(float)getWidth(),h=(float)getHeight();
 const auto gold=juce::Colour(0xffe6b77a), dim=juce::Colour(0xff7e8b9c), line=juce::Colour(0xff303b4a);
 g.fillAll(juce::Colour(0xff090d14));
 // Top chrome / brand identity
 g.setColour(juce::Colour(0xff131b27));g.fillRect(0,0,getWidth(),103);
 g.setColour(gold);g.fillRoundedRectangle(29.f,26.f,5.f,43.f,2.f);
 g.setColour(line);g.drawLine(25.f,102.f,w-25.f,102.f,1.f);
 g.setColour(dim);g.setFont(juce::Font(juce::FontOptions(11.f,juce::Font::bold)));
 g.drawText("PATCH LIBRARY",w-440.f,8.f,130.f,13.f,juce::Justification::centredLeft);
 // Left navigation rail, distinct from main surface
 g.setColour(juce::Colour(0xff101723));g.fillRoundedRectangle(24.f,119.f,166.f,h-240.f,12.f);
 g.setColour(line);g.drawRoundedRectangle(24.f,119.f,166.f,h-240.f,12.f,1.f);
 g.setColour(gold);g.fillRoundedRectangle(37.f,138.f,3.f,25.f,1.5f);
 g.setColour(dim);g.setFont(juce::Font(juce::FontOptions(10.f,juce::Font::bold)));
 g.drawText("WORKSPACE",45,128,130,15,juce::Justification::centredLeft);
 g.drawText("BASSFORGE  /  0.5",41,(int)h-170,140,18,juce::Justification::centredLeft);
 // Main upper visualizer
 const float mainX=207.f,mainW=w-mainX-24.f;
 auto hero=juce::Rectangle<float>(mainX,119.f,mainW,218.f);
 g.setColour(juce::Colour(0xff111a27));g.fillRoundedRectangle(hero,12.f);
 g.setColour(line);g.drawRoundedRectangle(hero,12.f,1.f);
 g.setColour(gold);g.setFont(juce::Font(juce::FontOptions(12.f,juce::Font::bold)));
 g.drawText(showFx?"EFFECTS  /  SIGNAL ARCHITECTURE":"OSCILLATOR  /  HARMONIC ENGINE",(int)mainX+23,136,420,23,juce::Justification::centredLeft);
 g.setColour(dim);g.setFont(juce::Font(juce::FontOptions(11.f)));
 g.drawText(showFx?"SHAPE  •  SATURATE  •  SPACE":"DUAL-MOTION FM  •  NEURO TIMBRE",(int)mainX+23,159,420,17,juce::Justification::centredLeft);
 // Stylized scope, intentionally a UI illustration rather than claiming real audio analysis
 const float x0=mainX+24.f, x1=mainX+mainW-25.f, y0=251.f;
 g.setColour(juce::Colour(0xff1d2938));
 for(int i=0;i<9;++i){float xx=x0+(x1-x0)*i/8.f;g.drawVerticalLine((int)xx,184.f,310.f);}
 for(int i=0;i<4;++i){float yy=192.f+i*35.f;g.drawHorizontalLine((int)yy,x0,x1);}
 juce::Path wave,glow;
 const float fm=processor.params.getRawParameterValue("fm")->load();
 const float move=processor.params.getRawParameterValue("movement")->load();
 for(int i=0;i<=450;++i){float t=i/450.f, x=x0+t*(x1-x0);
  float env=std::pow(std::sin(juce::MathConstants<float>::pi*t),.7f);
  float carrier=std::sin(t*juce::MathConstants<float>::twoPi*(7.f+fm*.06f)+std::sin(t*65.f)*(.3f+move*.008f));
  float y=y0-carrier*env*49.f;
  if(i==0){wave.startNewSubPath(x,y);glow.startNewSubPath(x,y);}else{wave.lineTo(x,y);glow.lineTo(x,y);}
 }
 g.setColour(gold.withAlpha(.12f));g.strokePath(glow,juce::PathStrokeType(10.f));
 g.setColour(gold);g.strokePath(wave,juce::PathStrokeType(2.f));
 // Main controls panel
 auto body=juce::Rectangle<float>(mainX,350.f,mainW,h-474.f);
 g.setColour(juce::Colour(0xff111a27));g.fillRoundedRectangle(body,12.f);
 g.setColour(line);g.drawRoundedRectangle(body,12.f,1.f);
 g.setColour(gold);g.setFont(juce::Font(juce::FontOptions(12.f,juce::Font::bold)));
 g.drawText(showFx?"FX CHAIN  /  07 MODULES":"SOUND DESIGN  /  12 MACROS",(int)mainX+23,363,410,22,juce::Justification::centredLeft);
 if(!showFx){
  float sep=mainX+mainW*.5f;
  g.setColour(line);g.drawVerticalLine((int)sep,398.f,h-146.f);
  g.setColour(dim);g.setFont(juce::Font(juce::FontOptions(10.f,juce::Font::bold)));
  g.drawText("OSCILLATION  +  MOTION",(int)mainX+25,390,(int)(mainW*.5f)-30,18,juce::Justification::centredLeft);
  g.drawText("SATURATION  +  OUTPUT",(int)sep+20,390,(int)(mainW*.5f)-35,18,juce::Justification::centredLeft);
 }else{
  const float rowH=(h-525.f)/7.f;
  for(int i=0;i<7;++i){float yy=398.f+i*rowH;g.setColour(i%2?juce::Colour(0xff182331):juce::Colour(0xff15202d));g.fillRoundedRectangle(mainX+14.f,yy,mainW-28.f,rowH-3.f,5.f);}
 }
 // Keyboard deck
 auto deck=juce::Rectangle<float>(24.f,h-110.f,w-48.f,91.f);
 g.setColour(juce::Colour(0xff151f2c));g.fillRoundedRectangle(deck,11.f);
 g.setColour(line);g.drawRoundedRectangle(deck,11.f,1.f);
 g.setColour(gold);g.setFont(juce::Font(juce::FontOptions(11.f,juce::Font::bold)));
 g.drawText("KEYS",41,(int)h-95,85,22,juce::Justification::centredLeft);
 g.setColour(dim);g.setFont(juce::Font(juce::FontOptions(10.f)));
 g.drawText("CLICK TO PLAY",41,(int)h-73,105,19,juce::Justification::centredLeft);
}
void BassforgeEditor::resized(){
 const int w=getWidth(),h=getHeight();
 title.setBounds(44,19,330,40);subtitle.setBounds(45,61,340,22);
 presets.setBounds(w-440,27,416,34);
 savePreset.setBounds(w-440,68,130,27);loadPreset.setBounds(w-302,68,130,27);loadPack.setBounds(w-164,68,140,27);
 synthTab.setBounds(39,159,137,48);fxTab.setBounds(39,216,137,48);
 shape.setBounds(39,290,137,33);bypass.setBounds(39,290,137,33);
 const float x=207.f, mw=(float)w-x-24.f;
 const float half=mw*.5f,cell=half/3.f;
 const float bodyHeight=(float)h-474.f;
 const float rowH=(bodyHeight-65.f)*.5f;
 for(int i=0;i<12;++i){
  const int section=i/6,local=i%6,col=local%3,r=local/3;
  float xx=x+section*half+col*cell, yy=412.f+r*rowH;
  knobs[(size_t)i].setBounds((int)(xx+6),(int)yy,(int)(cell-12),(int)juce::jmax(65.f,rowH-21.f));
  labels[(size_t)i].setBounds((int)(xx+4),(int)(yy+rowH-18.f),(int)(cell-8),19);
 }
 const float row=(h-525.f)/7.f;
 for(int i=0;i<7;++i){int y=(int)(398.f+i*row);
  fxLabels[(size_t)i].setBounds((int)x+25,y,160,(int)row-2);
  fxEnabled[(size_t)i].setBounds((int)x+184,y+2,35,(int)row-5);
  fxMix[(size_t)i].setBounds((int)x+224,y+2,(int)(mw*.28f),(int)row-5);
  fxAmount[(size_t)i].setBounds((int)(x+mw*.56f),y+2,(int)(mw*.23f),(int)row-5);
  fxUp[(size_t)i].setBounds(w-135,y+3,44,(int)row-6);
  fxDown[(size_t)i].setBounds(w-86,y+3,50,(int)row-6);
 }
 keyboard.setBounds(157,h-97,w-198,67);
}
BassforgeEditor::~BassforgeEditor(){setLookAndFeel(nullptr);}
juce::AudioProcessorEditor* BassforgeProcessor::createEditor(){return new BassforgeEditor(*this);}
