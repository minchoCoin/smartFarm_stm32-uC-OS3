# smartFarm_stm32-uC-OS3
임베디드시스템 과목의 텀프로젝트로, stm32f107vc board와 μC/OS-III를 사용하여 스마트팜을 제작하였습니다.

# 동작 영상 (사진을 클릭하면 동영상 링크가 클릭됩니다)
## 설명이 있는 영상
### 블루투스 통신
[![Video Label](http://img.youtube.com/vi/A5t4lj1mTuc/0.jpg)](https://youtube.com/shorts/A5t4lj1mTuc?feature=share)
### 온도 센서 및 히터 동작
[![Video Label](http://img.youtube.com/vi/aooRyYrrsaA/0.jpg)](https://youtu.be/aooRyYrrsaA)
### 토양습도 센서 및 펌프 동작
[![Video Label](http://img.youtube.com/vi/J_YHw7V48js/0.jpg)](https://youtu.be/J_YHw7V48js)
### 조도 센서 및 LED 동작
[![Video Label](http://img.youtube.com/vi/zbDKvlAY754/0.jpg)](https://youtu.be/zbDKvlAY754)
## 원본
### 블루투스 통신
[![Video Label](http://img.youtube.com/vi/VtmgRmLJn8c/0.jpg)](https://youtube.com/shorts/VtmgRmLJn8c?feature=share)
### 온도 센서 및 히터 동작
[![Video Label](http://img.youtube.com/vi/wUCVn_WxtwE/0.jpg)](https://youtu.be/wUCVn_WxtwE)
### 토양습도 센서 및 펌프 동작
[![Video Label](http://img.youtube.com/vi/t3J9c3ttH1c/0.jpg)](https://youtu.be/t3J9c3ttH1c)
### 조도 센서 및 LED 동작
[![Video Label](http://img.youtube.com/vi/bii7gK0_r1A/0.jpg)](https://youtu.be/bii7gK0_r1A)

# 목적
- 수업 시간에 배운 보드의 기능과 μC/OS-III 기능를 활용하여 스마트팜를 개발한다
- Bluetooth 및 통신 관련 기능을 이용하여 스마트팜를 개발한다.
- 온도 센서, 조도 센서, 토양 습도 센서를 이용하여 자동으로 농장을 관리하고 Bluetooth 모듈을 이용하여 농장의 상태를 수신할 수 있는 하드웨어를 개발한다.

# requirements
stm32f107vc board, μC/OS-III, temperature sensor(LM35DZ), Cds photoresistor sensor, soil moisture sensor, LED, flim heater, 2-channel relay module, pump, bluetooth module(HC-06)

# requirements(for bluetooth communication)
Android phone, [Serial bluetooth terminal App](https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=ko&gl=US&pli=1)

## 부품 구매한 곳
- temperature sensor: [링크](https://www.devicemart.co.kr/goods/view?no=3183)
- soil moisture sensor: [링크](https://www.devicemart.co.kr/goods/view?no=1376532)
- CDS photoresistor sensor: [링크](https://www.devicemart.co.kr/goods/view?no=10916353)
- LED(Red): [링크](https://www.devicemart.co.kr/goods/view?no=2851)
- LED(Blue): [링크](https://www.devicemart.co.kr/goods/view?no=1320882)
- film heater: [링크](https://www.devicemart.co.kr/goods/view?no=13192542)
- 알루미늄 방열판(히터에 부착): [링크](https://www.devicemart.co.kr/goods/view?no=13192547)
- 2-channel relay module: [링크](https://www.devicemart.co.kr/goods/view?no=1290043)
- 수중펌프모터: [링크](https://www.devicemart.co.kr/goods/view?no=1329770)
- 블루투스 모듈(HC-06) : [링크](https://www.devicemart.co.kr/goods/view?no=1278220)
- 온도 스위치(히터에 부착): [링크](https://www.devicemart.co.kr/goods/view?no=11569)

# 동작 시나리오
1. 스마트팜의 LED, 히터, 워터 펌프는 꺼진 상태이다.
2. 온도, 토양습도, 조도 임계값을 블루투스로 받는다.
3. 조도 센서로 스마트팜의 밝기를 측정한다.
4. 온도 센서로 스마트팜의 온도값을 측정한다.
5. 토양 습도 센서로 스마트팜 토양의 습도를 측정한다.
6. 시나리오 2, 3, 4 에서 측정한 온도, 습도, 밝기 값을 블루투스 모듈을 이용하여 휴대폰으로 전송한다.
7. 시나리오 2 에서 밝기가 임계값 이하로 떨어지면 LED 를 킨다.
8. 시나리오 3 에서 온도가 임계값 이하로 떨어지게 되면 릴레이 모듈을 통해 히터를 동작시킨다.
9. 시나리오 4 에서 습도가 임계값로 떨어지게 되면 릴레이 모듈을 통해 펌프를 동작시켜 물을 뿌린다.
10. 시나리오 7 에서 온도 스위치를 통해 히터의 온도가 65 도가 넘어가면 히터의 동작을 일시적으로 중지한다.

# microC/OS-III download
이 코드를 활용하기 위해서는 microC/OS-III 코드와 MDK-ARM IDE Keil μVision가 필요합니다. 코드 다운로드 방법 및 IDE 설치 방법은 [제 블로그](https://minchocoin.github.io/microc-os-3-stm32/2/) 에서 보실 수 있습니다.

# 핀 연결
- Temperatuer Sensor: 	PC1 (ADC1 Channel 11)
- SoilHumid Sensor: 		PC2 (ADC1 Channel 12)
- PhotoResistor Sensor: PC3(ADC1 Channel 13)
- Heater:								PD12(via relay module)
- Pump:									PD13(via relay module)
- LED:									PD9,PD10,PD11,PE10,PE11
- bluetooth:						PD5(USART2_TX), PD6(USART2_RX)

# 실행 방법
1. 보드와 각종 모듈을 연결합니다. 온도 센서, 토양습도센서, 조도 센서, LED, 블루투스 모듈를 연결하고, 히터와 펌프는 릴레이 모듈을 통해 연결합니다. 
2. microC/OS-III 코드를 다운로드하고, MDK-ARM IDE Keil μVision를 설치한 후,
```
코드다운받은 폴더\micrium_uc-eval-stm32f107_ucos-iii\Micrium\Software\EvalBoards\Micrium\uC-Eval-STM32F107\uCOS-III\KeilMDK\uCOS-III.uvproj
```
를 실행하여 app.c 와 app_cfg.h를 이 repository에 업로드되어있는 app.c와 app_cfg.h로 바꿉니다.
3. 
```
코드다운받은 폴더\micrium_uc-eval-stm32f107_ucos-iii\Micrium\Software\EvalBoards\Micrium\uC-Eval-STM32F107\uCOS-III\os_cfg_app.h
```
에서 OS_CFG_MSG_POOL_SIZE 을 150으로 늘려줍니다.
```c
#define  OS_CFG_MSG_POOL_SIZE            150u
```
3. project build(F7)과 download(F8)하여 보드에 포팅합니다.

4. 안드로이드 폰에서 블루투스와 연결 후, 블루투스 터미널 앱을 실행하여 보드와 통신을 시작합니다.
5. 블루투스 터미널 앱을 통해 온도 임계값, 토양습도 임계값, 조도 임계값을 입력합니다(온도의 단위는 섭씨, 토양습도의 단위는 % (높을 수록 습함), 조도센서의 단위는 아날로그값 그대로(0~4096, 높을 수록 어두움)입니다.
6. 온도, 토양습도, 조도 값이 표시되고, 평균값도 표시되며, 히터, 펌프, LED를 켤 때마다 블루투스 터미널에 표시됩니다.

# 코드 간략한 설명
## 생성한 Task
1. Start Task(우선순위 2)
- main에서 처음생성되는 task
- UART를 통해 온도 임계값, 습도 임계값, 조도센서 임계값을 받음
- 다른 task와 커널 객체를 만듦

2. Temper Task(우선순위 3)
- 측정한 온도값을 처리하는 task
- ADC mutex를 획득하면 온도센서에 맞추어 ADC설정
- ADC ISR로부터 task message queue를 통해 값을 받음
- 값을 섭씨 온도로 변환 및 온도값 정보를 Bluetooth task로 전송
- 온도값 정보를 Static Task에 전송
- 온도가 낮으면 온도가 낮다는 경고를 Bluetooth Task로 전송
- 온도가 낮으면 Heater Task에 신호를 보냄 (이미 작동중이라면 신호를 보내지 않음)

3. SoilHumid Task(우선순위 4)
- 측정한 토양습도값을 처리하는 task
- ADC mutex를 획득하면 습도센서에 맞추어 ADC설정
- 나머지는 Temper Task와 동일
- 습도가 낮으면 Pump Task에 신호를 보냄
   (이미 작동중이라면 신호를 보내지 않음)

4. Photoresistor Task(우선순위 5)
- 측정한 조도센서값을 처리하는 task
- ADC mutex를 획득하면 조도센서에 맞추어 ADC설정
- 나머지는 Temper Task와 동일
- 조도가 낮으면 light Task에 신호를 보냄
  (이미 작동중이라면 신호를 보내지 않음)
  
5. Static Task(우선순위 6)
-  센서값 통계 task
- TempQ, HumidQ, PhotoQ를 OS_OPT_PEND_NON_BLOCKING으로 기다려
  온도,습도, 조도 값을 받음
- 받은 값을 sum에 누적하고, 5개씩 모일 때마다 평균을 내어 평균값 정보를
  bluetooth task로 전송

6. Bluetooth Task(우선순위 2)
- 각종 task에서 받은 메시지를 Bluetooth로 전송하는 task

7. Heater Task(우선순위 2)
- 히터를 동작시키는 task(약 10초)

8. Pump Task(우선순위 2)
- 펌프를 동작시키는 Task(약 10초)

9. Light Task
- LED를 켜는 Task(약 10초)

## 생성한 커널 객체
1. ADC mutex
온도 센서, 습도 센서, 조도 센서가 하나의 ADC를 공유하기 위해 사용함. 어떤 센서가 ADC를 설정하고, ADC로부터 오는 인터럽트를 기다리고 있을 때, 다른 센서가 ADC 설정값을 건드릴 수 없도록 하기 위함 

2. TempQ message queue
static task에게 측정한 온도 값을 보내기 위해 필요

3. HumidQ message queue
Static task에게 측정한 습도 값을 보내기 위해 필요

4. PhotoQ message queue
Static task에게 측정한 조도 센서값을 보내기 위해 필요

5. OS_MEM
통계 task에게 실수값을 보낼 때, 블루투스 task에게 전송할 문자열을 보낼 때 필요. Message queue는 포인터밖에 전달할 수 없으므로, OS_MEM에서 메모리를 받아 해당 메모리에 값을 저장한 후, 포인터를 전달

## 사용한 μC/OS-III 기능
|        μC/OS-III 기능         |                                                                이유                                                               |
|:-----------------------------:|:---------------------------------------------------------------------------------------------------------------------------------:|
|        Task   management      |       여러 task를   만들어 multitasking을   하도록 하여 CPU 사용량을 극대화.     복잡한 하나의 task를   여러 개의 task로 분산     |
|     Interrupt   management    |     ADC를   통해 센서값   변환이 완료되었을 때, 블루투스로부터   값을 받았을 때 인터럽트를   발생시켜 이벤트에 실시간으로 대응    |
|        Message   Passing      |                                     센서 값 통계 task   및     블루투스 task에게   데이터 전송                                    |
|      Resource   management    |                                            센서가 하나의 ADC를        공유하기 위해 필요                                          |
|         Synchronization       |                                   센서값에   이상이 있을 때 해당하는 task에   신호를 보내어 대응                                  |
|       Memory   management     |                                         전송할 메시지의 내용을 담을 메모리를      할당받음                                        |
|        Time   management      |                                           일정 시간 간격으로 온도,   습도,   조도   측정                                          |

# 팀원
- 김태훈 [github](https://github.com/minchoCoin)
- 이성훈 [github](https://github.com/NextrPlue)

# Reference
- uC/OS-III: The Real-Time Kernel For the STM32 ARM Cortex-M3, Jean J. Labrosse, Micrium, 2009 [책 링크](https://micrium.atlassian.net/wiki/spaces/osiiidoc/overview?preview=/132386/157512/100-uCOS-III-ST-STM32-003.pdf)
- [STM32F107 Datasheet](https://www.st.com/resource/en/datasheet/stm32f107vc.pdf)
- [STM32F107 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- STM32F107VCT6 schematic
- [https://gopae02.tistory.com/18](https://gopae02.tistory.com/18)
- [https://copyprogramming.com/howto/c-convert-string-to-float-f-code-example](https://copyprogramming.com/howto/c-convert-string-to-float-f-code-example)
- [https://blog.naver.com/aul-_-/221485012562](https://blog.naver.com/aul-_-/221485012562)
