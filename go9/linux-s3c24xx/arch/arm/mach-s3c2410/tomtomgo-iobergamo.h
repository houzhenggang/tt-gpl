#define SUPPORT_BERGAMO

static backlight_mapping_t bergamo_bl_mapping = {17,22,26,31,34,36,38,40,42,45,47,
	                49,52,54,57,59,62,64,67,70,72};

#ifdef __BOOTLOADER__
void IO_DetectFeature(void);
#endif

inline void IO_InitBergamo(void)
{
	VALUE_SET(id                 , GOTYPE_BERGAMO);
	VALUE_SET(caseid             , GOCASE_ROME);
	VALUE_SET(cputype            , GOCPU_S3C2450);

	VALUE_SET(batterytype	     , GOBATTERY_LISHEN_900);// need to check
	VALUE_SET(chargertype        , GOCHARGER_LTC3455);
	VALUE_SET(battcalibration    , 1);
	VALUE_SET(chargerresistor    , 3000); /* changed in PR1 */
	VALUE_SET(internalflash      , 1);	
	VALUE_SET(hsmmcinterface0_4bit , 1);
	VALUE_SET(hsmmcclocktype     , GOHSMMC_USB48CLK);
#ifdef __BOOTLOADER__
	VALUE_SET(movi_hsmmc_port    , 0);
	VALUE_SET(alt_lcd_controller , 1); /* use alt. lcd controller */
#endif
	VALUE_SET(tfttype            , GOTFT_SAMSUNG_LMS430HF19);
	VALUE_SET(backlighttype	     , GOBACKLIGHT_FB_CH1_CAT4238TD_430);
	VALUE_SET(backlightfreq      ,20000);
	VALUE_SET(backlight_inverted , 1);
	VALUE_SET(backlight_mapping  , &bergamo_bl_mapping);
#ifdef __KERNEL__
	VALUE_SET(backlighttimer     , 1);	/* kernel uses other timer for PWM */
#endif
	VALUE_SET(gpstype            , GOGPS_GL_BCM4750);
	VALUE_SET(codectype          , GOCODEC_ALC5628);
	VALUE_SET(codecmaster        , GOCODECCFG_SLAVE);
	VALUE_SET(usbslavetype       , GOUSB_S3C2443);
	VALUE_SET(ohciports          , 0x01);
	VALUE_SET(dualusbphyctrl     , 1); 
	VALUE_SET(usbdevicehostcapable,1);
	VALUE_SET(regulatorforcodec  , 1);
	VALUE_SET(keeprtcpowered     , 1);
	VALUE_SET(hw_i2c             , 1);
	VALUE_SET(pnp                , 1);
	VALUE_SET(loquendo           , 0);
	VALUE_SET(gpsephemeris       , 1);
#ifdef __BOOTLOADER__
	VALUE_SET(altchargechannel   , 5);
#else
	VALUE_SET(batchargecurrentchannel, 5);
#endif
	VALUE_SET(battvoltnum        , 7);
	VALUE_SET(battvoltdenom      , 5);	
	VALUE_SET(tsxchannel         , 7);
	VALUE_SET(tsychannel         , 9);
	VALUE_SET(tsdownchannel      , 9);
	VALUE_SET(gldetected         , 1);
	VALUE_SET(tftsoftstart       , 1);
	VALUE_SET(sixbuttonui        , 0);

	STRING_SET(familyname,"TomTom ONE"); 
	STRING_SET(projectname,"Bergamo");
	STRING_SET(requiredbootloader,"5.4220");
	STRING_SET(gpsdev,"ttySAC0");
	STRING_SET(dockdev,"ttySAC3");

	STRING_SET(name,"TomTom XL");
	STRING_SET(usbname,"XL");

  //gps
	PIN_SET(GPS_RESET          , PIN_GPB0 | PIN_INVERTED);
	//PIN_SET(GPS_1PPS           , PIN_GPG0);
	PIN_SET(GPS_STANDBY        , PIN_GPH13 | PIN_INVERTED);
	//adc
	// removed in PR1: PIN_SET(AIN4_PWR           , PIN_GPL13);
  //codec
	PIN_SET(I2SSDO             , PIN_GPE4);
	PIN_SET(CDCLK              , PIN_GPE2);
	PIN_SET(I2SSCLK            , PIN_GPE1);
	PIN_SET(I2SLRCK            , PIN_GPE0);
  //debug UART
	PIN_SET(TXD_DOCK           , PIN_GPH6);
	PIN_SET(RXD_DOCK           , PIN_GPH7);
	PIN_SET(RXD_DOCK_INT       , PIN_GPG2);
  //GPS UART
	PIN_SET(TXD_GPS            , PIN_GPH0);
	PIN_SET(RXD_GPS            , PIN_GPH1);		
	PIN_SET(RTS_GPS            , PIN_GPH9);
	PIN_SET(CTS_GPS            , PIN_GPH8);
  //LCD
	PIN_SET(LCD_VCC_PWREN      , PIN_GPA19| PIN_INVERTED);
	PIN_SET(LCD_ID             , PIN_GPB9 );
	PIN_SET(LCD_RESET          , PIN_GPF4 | PIN_INVERTED);

	PIN_SET(ON_OFF             , PIN_GPF0 | PIN_INVERTED);
	PIN_SET(ACPWR              , PIN_GPF2 | PIN_INVERTED);
	PIN_SET(USB_HOST_DETECT    , PIN_GPF2 | PIN_INVERTED);
	PIN_SET(IGNITION           , PIN_GPF2 | PIN_INVERTED);
	PIN_SET(CHARGING           , PIN_GPG1 | PIN_INVERTED);
	
  //usb phy pwr
	PIN_SET(USB_PHY_1V2_PWR_EN , PIN_GPA9);
	PIN_SET(USB_PHY_PWR_EN	   , PIN_GPA8);
	
	//pmic
	PIN_SET(PWR_RST            , PIN_GPG5);
	PIN_SET(USB_PWR_BYPASS     , PIN_GPG4);//WALL ON
	PIN_SET(BATT_TEMP_OVER     , PIN_GPG3 | PIN_INVERTED);
	PIN_SET(USB_SUSPEND_OUT    , PIN_GPG6);
	//PIN_SET(HSON    , PIN_GPG7); now reserved	==> HS not used	
  //iic
	PIN_SET(HW_IIC_SDA         , PIN_GPE15);
	PIN_SET(HW_IIC_SCL         , PIN_GPE14);
  //backlight
	PIN_SET(BACKLIGHT_PWM      , PIN_GPB1);
	PIN_SET(BACKLIGHT_EN       , PIN_GPF7);
	//lcd spi
	PIN_SET(LCD_CS             , PIN_GPC0 | PIN_INVERTED);
	PIN_SET(LCD_SCL            , PIN_GPE13);
	PIN_SET(LCD_SDI            , PIN_GPE12);	
}

inline void IO_InitBergamoUS(void)
{
        STRING_SET(familyname,"TomTom EASE");
        STRING_SET(name,"TomTom EASE");
        STRING_SET(usbname,"EASE");
}

inline void IO_InitChelsea4(void)
{
        VALUE_SET(id            , GOTYPE_CHELSEA4);
        STRING_SET(projectname,"Chelsea4");
        STRING_SET(familyname,"TomTom START");
        STRING_SET(name,"TomTom START");
        STRING_SET(usbname,"START");
}

