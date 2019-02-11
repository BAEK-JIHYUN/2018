#include <FreeRTOS_AVR.h>

#define SignalPin1 53     // 검
#define SignalPin2 47     // 빨
#define SignalPin3 37     // 노

#define Blank_JudgeTime 1200      // 이상 입력이 들어오지 않아야 빈 점자로 판단.
#define Nosignal_EndTime 9000     // 이상 입력이 없으면 입력이 종료되었다고 판단.
#define Debouncing_Time 300

const unsigned char Initial[64] = { 0, 7, 4, 10, 1, 9, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 0, 0, 3, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                           0, 5, 0, 2, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                           0, 12, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
                           
const unsigned char Medial[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 107, 111, 103, 0, 0,
                          0, 0, 0, 0, 115, 0, 116, 0, 0, 0, 0, 104, 0, 0, 0, 0, 
                          0, 0, 106, 0, 114, 0, 0, 0, 108, 117, 0, 112, 0, 105, 0, 110,
                          101, 0, 0, 0, 0, 0, 0, 0, 109, 102, 0, 113, 0, 0, 0 };
                          
const unsigned char Final[64] = { 0, 0, 0, 0, 0, 0, 0, 0, 207, 0, 203, 214, 0, 0, 0, 0, 204,
                         205, 202, 213, 0, 0, 0, 0, 210, 212, 211, 208, 0, 0, 0, 0, 201, 
                         0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 206,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; 

                         /*
                            * ㄱ1  201   ㅏ101
                            * ㄴ2  202   ㅐ102
                            * ㄷ3  203   ㅑ103
                            * ㄹ4  204   ㅓ104
                            * ㅁ5  205   ㅔ105
                            * ㅂ6  206   ㅕ106
                            * ㅅ7  207   ㅖ107
                            * ㅇ8  208   ㅗ108
                            * ㅈ9  209   ㅘ109
                            * ㅊ10 210   ㅚ110
                            * ㅋ11 211   ㅛ111
                            * ㅌ12 212   ㅜ112
                            * ㅍ13 213   ㅝ113
                            * ㅎ14 214   ㅠ114
                            *            ㅡ115
                            *            ㅢ116
                            *            ㅣ117
                       */                     


int blank_counter = 0; // 빈 점자인지 점자와 점자 사이의 공간이지 판단하기 위한 counter 
int nosignal_counter = 0; // 일정 시간 이상 입력이 들어오지 않으면 입력 종료를 알리기 위한 counter
int readIndex = 0; // inputBuffer에 저장된 데이터 처음부터 끝까지 순차적으로 읽는 인덱스
int speakIndex = 0; // speakBuffer에 저장된 데이터 처음부터 끝까지 순차적으로 읽는 인덱스

unsigned char inputBuffer[100] = {0}; // 점자로 해석할 데이터 저장을 위한 
unsigned char speakBuffer[100] = {0}; // 음성으로 출력할 데이터 저장을 위한 
int inputBufferindex = 0;
int speakBufferindex = 0;
int inputArr[6] = {0}; // 점자 입력 6개 저장을 위한 Array
int inputIndex = 0; // inputArr의 마지막 데이터 위치를 저장하는 변수

//int BUSY = 5;   // JP1-5
int PLAY = 6;   // JP1-6
int NEXT = 7;   // JP1-7
//int PREV = 8;   // JP1-1
//int DIN = 4;    // JP1-4
//int DCLK = 3;   // JP1-3
int RESET = 2;  // JP2-5

/*  
 *  JP2-1  PWM-OUT
 *  JP2-6  GND
 *  JP2-7  3.3V
 *  JP2-5  RESET  -> reset 시에 노래 0001번부터 시작하게해줌
 */


void InputTask(void)
{ 
    // inputArr에 데이터 입력     
    int p1 = digitalRead(SignalPin1);
    int p2 = digitalRead(SignalPin2);
    int p3 = digitalRead(SignalPin3);


    if( (p1 == 0) || (p2 == 0) || (p3 == 0) )  // 핀 하나라도 입력이 들어오면 counter 초기화 뒤 input 함수 수행
    {   
        nosignal_counter = 0;
        blank_counter = 0;
        input(p1, p2, p3);
        delay(Debouncing_Time);
    }
    
    else 
    { 
      if( (blank_counter > Blank_JudgeTime) ) // 하나도 입력이 들어오지 않았고 빈 점자로 판단되면 inputArr에 0 0 0 입력 후 input_detected를 0으로 변경
      {                          
        blank_counter=0;            
        inputArr[inputIndex++] = 0;
        inputArr[inputIndex++] = 0;
        inputArr[inputIndex++] = 0;
      }
      delay(Debouncing_Time);

    }

    if ( inputIndex > 5 )
    {
      inputIndex = 0;
      input_BinToDec();
    }
    
    blank_counter += Debouncing_Time;    
    nosignal_counter += Debouncing_Time;
}


// inputArr에 점자입력을 저장하는 함수
void input(int p1, int p2, int p3)
{  
    if ( (p1 == 1) && (p2 == 1) && (p3 == 0) ) // 입력이 0 0 1 인 경우
    {
      inputArr[inputIndex++] = 0;
      inputArr[inputIndex++] = 0;
      inputArr[inputIndex++] = 1;
      Serial.print("001");
    }

    if ( (p1 == 1) && (p2 == 0) && (p3 == 1) ) // 입력이 0 1 0 인 경우
    {
      inputArr[inputIndex++] = 0;
      inputArr[inputIndex++] = 1;
      inputArr[inputIndex++] = 0;
      Serial.print("010");
    }

    if ( (p1 == 1) && (p2 == 0) && (p3 == 0) ) // 입력이 0 1 1 인 경우
    {
      inputArr[inputIndex++] = 0;
      inputArr[inputIndex++] = 1;
      inputArr[inputIndex++] = 1;
      Serial.print("011");
    }

    if ( (p1 == 0) && (p2 == 1) && (p3 == 1) ) // 입력이 1 0 0 인 경우
    {
      inputArr[inputIndex++] = 1;
      inputArr[inputIndex++] = 0;
      inputArr[inputIndex++] = 0;
      Serial.print("100");
    }

    if ( (p1 == 0) && (p2 == 1) && (p3 == 0) ) // 입력이 1 0 1 인 경우
    {
      inputArr[inputIndex++] = 1;
      inputArr[inputIndex++] = 0;
      inputArr[inputIndex++] = 1;
      Serial.print("101");
    }

    if ( (p1 == 0) && (p2 == 0) && (p3 == 1) ) // 입력이 1 1 0 인 경우
    {
      inputArr[inputIndex++] = 1;
      inputArr[inputIndex++] = 1;
      inputArr[inputIndex++] = 0;
      Serial.print("110");
    }

    if ( (p1 == 0) && (p2 == 0) && (p3 == 0) ) // 입력이 1 1 1 인 경우
    {
      inputArr[inputIndex++] = 1;
      inputArr[inputIndex++] = 1;
      inputArr[inputIndex++] = 1;
      Serial.print("111");
    }
    Serial.print("\n");
}


// InputTask에서 6개의 입력을 2진수에서 10진수로 바꿔주는 함수
void input_BinToDec(void) 
{
  Serial.print("Arr : ");
  for(int i=0; i<6; i++)
  {
    Serial.print(inputArr[i]);
  }
  Serial.print("\n");
  
  int input_sum = inputArr[5]*1 + inputArr[4]*2 + inputArr[3]*4 + inputArr[2]*8 + inputArr[1]*16 + inputArr[0]*32; // 6개 점자의 10진수 변환 값을 저장하는 변수

  if ( Initial[input_sum] != 0 ) inputBuffer[inputBufferindex++] = Initial[input_sum];
  else if ( Medial[input_sum] != 0 ) inputBuffer[inputBufferindex++] = Medial[input_sum];
  else if ( Final[input_sum] != 0 ) inputBuffer[inputBufferindex++] = Final[input_sum];
}




// 초성, 모음, 종성을 확인하는 함수
int detectData(int a)
{
  if( (a > 0) && (a < 100) )// 초성 
    return 1;
  else if ( (a >= 100) && (a < 200) )// 모음
    return 2;
  else if ( (a >= 200) && (a < 300) )// 종성 
    return 3; 
}

// 데이터를 합쳐서 글자로 만드는 함수 (파라미터 1개)
int combineData(int a)
{
  switch(a)
  { 
    case 111: Serial.print("요\n"); return 6; break; // "요"인 경우 6번 파일 play

    case 117: Serial.print("이\n"); return 10; break; // "이"인 경우 10번 파일 play
  }
}

// 데이터를 합쳐서 글자로 만드는 함수 (파라미터 2개)
int combineData(int a, int b)
{
  if( (a >= 100) && (a < 200) ) // 모음으로 시작
  {
     switch(a)
     {
       case 101: 
          switch(b)
          {
            case 202: Serial.print("안\n"); return 2; break; // "안"인 경우 2번 파일 play
          }
          break;
     } 
  }

  else if ( (a > 0) &&  (a < 100) ) // 초성으로 시작
  {
  
    switch(a)
     {
        case 3:
          switch(b)
          {
            case 101: Serial.print("다\n"); return 8; break; // "다"인 경우 8번 파일 play
          }
          break; 

        case 5:
          switch(b)
          {
            case 117: Serial.print("미\n"); return 7; break; // "미"인 경우 7번 파일 play
          }
          break; 
          
        case 7:
          switch(b)
          {
            case 105: Serial.print("세\n"); return 5; break; // "세"인 경우 5번 파일 play
          }
          break;  
         
       case 14: 
          switch(b)
          {
            case 101: Serial.print("하\n"); return 4; break; // "하"인 경우 4번 파일 play
          }
          break;
     }
     
  }
}

// 데이터를 합쳐서 글자로 만드는 함수 (파라미터 3개)
int combineData(int a, int b, int c)
{
   switch(a)
     {
       case 2: 
          switch(b)
          {
            case 101: 
              switch(c)
              {
                case 204: Serial.print("날\n"); return 9; break; // "날"인 경우 9번 파일 play
              }
            break;
            
            case 106: 
              switch(c)
              {
                case 208: Serial.print("녕\n"); return 3; break; // "녕"인 경우 3번 파일 play
              }
            break;
          }
          break;

         
       case 9: 
          switch(b)
          {
            case 108: 
              switch(c)
              {
                case 214: Serial.print("좋\n"); return 11; break; // "좋"인 경우 11번 파일 play
              }
            break;

     }
  }
}

void ReadTask()
{

      // 글자의 첫 시작이 모음이고 다음 데이터가 초성이거나 없으면 한 글자를 이루는 데 하나의 데이터 사용
      if( (detectData(readIndex) == 2) && ((detectData(readIndex+1) == 1) || (readIndex >= inputBufferindex)) ) 
      {
        speakBuffer[speakBufferindex] = combineData(inputBuffer[readIndex]);
        speakBufferindex++;
          
        readIndex += 1;
      }
        
      // 글자의 첫 시작이 모음이고 다음 데이터가 종성, 그 다음 데이터가 초성이거나 없으면 한 글자를 이루는 데 두 개의 데이터 사용
      else if ( (detectData(readIndex) == 2) && (detectData(readIndex+1) == 3) && ((detectData(readIndex+2 == 1) || (readIndex+2 >= inputBufferindex)) ) ) 
      {
        speakBuffer[speakBufferindex] = combineData(inputBuffer[readIndex], inputBuffer[readIndex+1]);
        speakBufferindex++;
          
        readIndex += 2;
      }

      // 글자의 첫 시작이 초성이고 다음 데이터가 모음, 그 다음 데이터가 초성이거나 없으면 한 글자를 이루는 데 두 개의 데이터 사용
      else if ( (detectData(readIndex) == 1) && (detectData(readIndex+1) == 2) && ((detectData(readIndex+2) == 1) || (readIndex+2 >= inputBufferindex)) ) 
      {
        speakBuffer[speakBufferindex] = combineData(inputBuffer[readIndex], inputBuffer[readIndex+1]);
        speakBufferindex++;
        
        readIndex += 2;
      }
        
      // 글자의 첫 시작이 초성이고 다음 데이터가 모음, 그 다음 데이터가 종성, 그 다음 데이터가 초성이거나 없으면 한 글자를 이루는 데 세 개의 데이터 사용 
      else if ( (detectData(readIndex) == 1) && (detectData(readIndex+1) == 2) && (detectData(readIndex+2) == 3) && ((detectData(readIndex+3) == 1) || (readIndex+3 >= inputBufferindex)) ) 
      {
        speakBuffer[speakBufferindex] = combineData(inputBuffer[readIndex], inputBuffer[readIndex+1], inputBuffer[readIndex+2]);
        speakBufferindex++;  
        
        readIndex += 3;
      }

      // 오류나 점자 제작자의 실수 등으로 해당되는 카테고리가 없는 경우 
      else 
      {
        readIndex += 1;  
      }
}




void SpeakTask()
{       
  
    digitalWrite(RESET, LOW);
    delay(100);
    digitalWrite(RESET, HIGH);
    delay(100);
      
    digitalWrite(PLAY, HIGH);
    delay(200);
    digitalWrite(PLAY, LOW);
    delay(200);
    
    for(int i = 0; i < speakBuffer[speakIndex]; i++)
    { 
      digitalWrite(NEXT, HIGH);
      delay(150);
      digitalWrite(NEXT, LOW);
      delay(150);
    } 
    
    digitalWrite(PLAY, HIGH);
    delay(750); 
    digitalWrite(PLAY, LOW);
    delay(10); 
    
    speakIndex ++;
}


void setup() 
{
  pinMode(SignalPin1, INPUT_PULLUP);
  pinMode(SignalPin2, INPUT_PULLUP);
  pinMode(SignalPin3, INPUT_PULLUP);
  pinMode(PLAY, OUTPUT);
  pinMode(NEXT, OUTPUT);
  pinMode(RESET, OUTPUT);

  Serial.begin (9600);
}


void loop() 
{
      while(1)
      {
        InputTask();
        
        if( nosignal_counter > Nosignal_EndTime ) // nosignal_counter가 Nosignal_EndTime을 넘으면 입력이 종료되었다고 판단하여 ReadTask 수행 
        {
          Serial.println("Input Task end");
        
          Serial.println("----Input Buffer----");
          for(int i=0; i<10; i++)
          {
            Serial.print(inputBuffer[i]);
            Serial.print(" ");
          }
          Serial.print("\n");
        
          nosignal_counter = 0;
          break;
        }
      }  
      
      while(1)
      {
        ReadTask();

        if(readIndex >= inputBufferindex)
        {
          Serial.println("Read Task end");
          
          Serial.println("----Speak Buffer----");
          for(int i=0; i<10; i++)
          {
            Serial.print(speakBuffer[i]);
            Serial.print(" ");
          }
          Serial.print("\n");
          
          inputBufferindex = 0;
          readIndex = 0;
          break;
        }
      }

      while(1)
      {
        SpeakTask();
      
        if(speakIndex >= speakBufferindex)
        {
          Serial.println("Speak Task end");
        
          speakBufferindex = 0;
          speakIndex = 0;
          break;
        }
      }
      
}
