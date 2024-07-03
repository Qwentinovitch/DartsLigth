#define ROUGE_PIN 5
#define BLEU_PIN 3
#define VERT_PIN 6
#define BTN_PIN 4

#define TIMEOUT_CHANGE 1000
#define TIMEOUT 5000

enum BTN_Etat{
  INIT,
  COLOR,
  DIMM,
  BLINK,
  CHANGE_BLINK,
  CHANGE_DIMM,
  CHANGE_COLOR
};

enum Color_List{
  NONE,
  ROUGE,
  BLEU,
  VERT,
  VIOLET,
  JAUNE,
  MAGENTA,
  FULL
};

enum Blink_List{
  FIX,
  CHANGING
};

const int RougeC[] =    {0, 254, 254};
const int BleuC[] =     {254, 0  , 254};
const int VertC[] =     {254, 254, 0  };
const int VioletC[] =   {0  , 0  , 254};
const int JauneC[] =    {0  , 254, 0  };
const int MagentaC[] =  {254, 0  , 0  };
const int FullC[] =     {0  , 0  , 0  };
const int NoneC[] =     {254, 254, 254};
int* Color[] = {NoneC, RougeC, BleuC, VertC, VioletC, JauneC, MagentaC, FullC};

const int Intesitee[] = {0, 50 , 100, 150, 200};

int select, first_pass;
unsigned long time_BTN, timeout_State, SecCountTEST, SecCount, SecCountBlink;
int DimmR, DimmB, DimmV, Current_Color, Blink, Time_Diff, Dimmable, UpsideDown, BlinkValue;
int BTN_Etat;
const int Color_count = FULL, Blink_count = CHANGING+1, Dim_count = 4;


void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  pinMode(BLEU_PIN, OUTPUT);
  pinMode(ROUGE_PIN, OUTPUT);
  pinMode(VERT_PIN, OUTPUT);
  pinMode(BTN_PIN, INPUT);
  select = FULL;
  BTN_Etat = INIT;
  Blink = FIX;
  BlinkValue = 254;
  time_BTN = 0;
  Dimmable = 0;
}

void loop() {
  State_Machine();
  Blinking(Blink);
}
int CheckValue(int Value)
{
  if(Value < 0){
    Value = 0;
  }
  return Value;
}

int ChangeColor(int Selection)
{
  Selection--;
  if(Selection <= 0){
    Selection = Color_count;
  }
  return Selection;
}

int ChangeBlink(int Value)
{
  Value++;
  if(Value >= Blink_count){
    Value = 0;
  }
  if(Value == CHANGING){
    Serial.println("CHANGING");
  }else{
    Serial.println("FIX");
  }
  return Value;
}

int ChangeDimmable(int Dimm)
{
  Dimm++;
  if(Dimm > Dim_count){
    Dimm = 0;
  }
  return Dimm;
}

void Blinking(int Value){
  if(Value == CHANGING){
    if((SecCountBlink + 10 + (Intesitee[Dimmable]/5)) <= millis()){
      SecCountBlink = millis();
      if(UpsideDown == 0){
        BlinkValue++;
        if(BlinkValue >= 254){
          UpsideDown = 1;
          select = ChangeColor(select);
        }
      }else{
        BlinkValue--;
        if((BlinkValue <= Intesitee[Dimmable])){
           UpsideDown = 0;
           Serial.print("blink Changing  ");
           Serial.println(Intesitee[Dimmable]);
        }
      }
      writeLEDS();
    }
  }else if(Value == FIX){
    BlinkValue = 0;
    if(BlinkValue < Intesitee[Dimmable]){
      BlinkValue = Intesitee[Dimmable];
      UpsideDown = 0;
    }
  }
}

void writeLEDS()
{
  DimmR = CheckValue(Color[select][0]);
  DimmB = CheckValue(Color[select][1]);
  DimmV = CheckValue(Color[select][2]);

  if(Blink != FIX){
     if(DimmR == 0){
      DimmR = BlinkValue;
    }
    if(DimmB == 0){
      DimmB = BlinkValue;
    }
    if(DimmV == 0){
      DimmV = BlinkValue;
    }
  }else{
   if(DimmR == 0){
      DimmR = Intesitee[Dimmable];
    }
    if(DimmB == 0){
      DimmB = Intesitee[Dimmable];
    }
    if(DimmV == 0){
      DimmV = Intesitee[Dimmable];
    } 
  }
  
  analogWrite(ROUGE_PIN, DimmR);
  analogWrite(BLEU_PIN, DimmB);
  analogWrite(VERT_PIN, DimmV);

  if((SecCountTEST + 500) <= millis()){
    SecCountTEST = millis();
    Serial.print("Rouge : ");
    Serial.println(DimmR);
    Serial.print("Bleu  : ");
    Serial.println(DimmB);
    Serial.print("Vert  : ");
    Serial.println(DimmV);
  }
}

void State_Machine()
{
  switch(BTN_Etat){
    case INIT:
    first_pass = 1;
    select = FULL;
    writeLEDS();
    delay(200);
    select = ROUGE;
    writeLEDS();
    delay(200);
    select = BLEU;
    writeLEDS();
    delay(200);
    select = VERT;
    writeLEDS();
    delay(200);
    select = VIOLET;
    writeLEDS();
    delay(200);
    select = JAUNE;
    writeLEDS();
    delay(200);
    select = MAGENTA;
    writeLEDS();
    delay(200);
    Current_Color = FULL;
    select = Current_Color;
    writeLEDS();
    delay(200);
    break;
    case COLOR:
    if(first_pass == 1){
      select = Current_Color;
      writeLEDS();
      delay(100);
      select = NONE;
      writeLEDS();
      delay(100);
      select = Current_Color;
      writeLEDS();
      delay(100);
      select = NONE;
      writeLEDS();
      delay(100);
      select = Current_Color;
      writeLEDS();
      first_pass = 0;
      Blink = FIX;
      BlinkValue = 0;
    }
    break;
    case DIMM:
    select = BLEU;
    writeLEDS();
    break;
    case BLINK:
    if(Blink == FIX){
      select = ROUGE;
      writeLEDS();
      BlinkValue = Intesitee[Dimmable];
    }
    break;
    case CHANGE_BLINK:
    first_pass = 1;
    Blink = ChangeBlink(Blink);
    break;
    case CHANGE_DIMM:
    first_pass = 1;
    Dimmable = ChangeDimmable(Dimmable);
    break;
    case CHANGE_COLOR:
    select = ChangeColor(Current_Color);
    Current_Color = select;
    writeLEDS();
    break;
  }

  if (BTN_Etat == INIT)
  {
    BTN_Etat = COLOR;
    Serial.println("INIT -> COLOR ");
  }
  else if (BTN_Etat == COLOR)
  {
    if (digitalRead(BTN_PIN) == 1){
      if (time_BTN == 0){
        time_BTN = millis(); 
      }
    }else{
      if (time_BTN != 0){
        Time_Diff = millis() - time_BTN;
        Serial.print("Diff ");
        Serial.println(Time_Diff);
        if (Time_Diff >= TIMEOUT_CHANGE){
          Serial.println("COLOR -> DIMM ");
          BTN_Etat = DIMM;
          time_BTN = 0;
          first_pass = 1;
        }else{
          Serial.println("COLOR -> CHANGE_COLOR ");
          BTN_Etat = CHANGE_COLOR;
          time_BTN = 0;
        } 
      }     
    }
  }
  else if (BTN_Etat == DIMM)
  {
    if(first_pass == 1){
      timeout_State = millis();
      first_pass = 0;
    }
    else{
      Time_Diff = timeout_State - millis();
      if(Time_Diff >= TIMEOUT){
        BTN_Etat = COLOR;
        Serial.println("DIMM -> COLOR ");
      }    
      if (digitalRead(BTN_PIN) == 1){
        if (time_BTN == 0){
          time_BTN = millis();
        }
      }else{
        if (time_BTN != 0){
          Time_Diff = millis() - time_BTN;
          if (Time_Diff >= TIMEOUT_CHANGE){
            Serial.println("DIMM -> BLINK ");
            BTN_Etat = BLINK;
            time_BTN = 0;
            first_pass = 1;
          }else{
            Serial.println("DIMM -> CHANGE_DIMM ");
            BTN_Etat = CHANGE_DIMM;
            time_BTN = 0;
            first_pass = 1;
          }
        }
      }
    }
  }
  else if (BTN_Etat == BLINK)
  {
    /*if(first_pass == 1){
      timeout_State = millis();
      first_pass = 0;
      
    }
    Time_Diff = timeout_State - millis();
    if(Time_Diff >= TIMEOUT){
      BTN_Etat = COLOR;
      Serial.println("BLINK -> COLOR ");
    }*/
    
    if (digitalRead(BTN_PIN) == 1){
      if (time_BTN == 0){
        time_BTN = millis();
      }
    }else{
      if (time_BTN != 0){
        Time_Diff = millis() - time_BTN;
        if (Time_Diff >= TIMEOUT_CHANGE){
          Serial.println("BLINK -> COLOR ");
          BTN_Etat = COLOR;
          time_BTN = 0;
          first_pass = 1;
        }else{
          Serial.println("BLINK -> CHANGE_BLINK ");
          BTN_Etat = CHANGE_BLINK;
          time_BTN = 0;
        }
      }  
    }
  }
  else if (BTN_Etat == CHANGE_BLINK)
  {
    delay(1);
    if (digitalRead(BTN_PIN) == 0){
      Serial.println("CHANGE_BLINK -> BLINK");
      BTN_Etat = BLINK;
    }
    
  }
  else if (BTN_Etat == CHANGE_DIMM)
  {
    delay(1);
    if (digitalRead(BTN_PIN) == 0){
      Serial.println("CHANGE_DIMM -> DIMM");
      BTN_Etat = DIMM;
    }
    
  }
  else if (BTN_Etat == CHANGE_COLOR)
  {
    delay(1);
    if (digitalRead(BTN_PIN) == 0){
      Serial.println("CHANGE_COLOR -> COLOR");
      BTN_Etat = COLOR;
    }
  }
}
