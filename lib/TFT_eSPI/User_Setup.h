// Refer to the modified library, TFT_eSPI/User_Setup_original.h for the original file with all comments.

#define USER_SETUP_INFO "User_Setup"
#define ST7789_DRIVER     
#define TFT_RGB_ORDER TFT_BGR  
#define TFT_WIDTH  240 
 
#define TFT_HEIGHT 240 
#define TFT_INVERSION_ON
#define TFT_CS   PIN_D8  
#define TFT_DC   PIN_D3  
#define TFT_RST  PIN_D4  
#define TFT_BL PIN_D1  
#define TFT_MOSI  41   
#define TFT_SCLK  40 
#define TFT_CS    39  
#define TFT_DC    38 
#define TFT_RST   42  
#define TFT_BL   20  
                        
                        
#define LOAD_GLCD 
#define LOAD_FONT2 &Satisfy_24
#define LOAD_FONT4 &Satisfy_24
#define LOAD_FONT6 
#define LOAD_FONT7 
#define LOAD_FONT8  
#define LOAD_GFXFF  
#define SMOOTH_FONT
 
#define SPI_FREQUENCY  40000000
    
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
