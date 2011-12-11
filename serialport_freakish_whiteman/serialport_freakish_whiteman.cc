// Copyright 2011 Chris Williams <chris@iterativedesigns.com>

#include "serialport_native.h"
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */

#include <windows.h>
#include <tchar.h>

#include <node.h>    /* Includes for JS, node.js and v8 */
#include <node_buffer.h>
#include <v8.h>


#define THROW_BAD_ARGS ThrowException(Exception::TypeError(String::New("Bad argument")))


namespace node {

  using namespace v8;
  
  static Persistent<String> errno_symbol;

  static Handle<Value> Read(const Arguments& args) {
    HandleScope scope;

    if (!args[0]->IsInt32())  {
      return scope.Close(THROW_BAD_ARGS);
    }
    int fd = args[0]->Int32Value();


    char * buf = NULL;

    if (!Buffer::HasInstance(args[1])) {
      return ThrowException(Exception::Error(
                  String::New("Second argument needs to be a buffer")));
    }

    Local<Object> buffer_obj = args[1]->ToObject();
    char *buffer_data = Buffer::Data(buffer_obj);
    size_t buffer_length = Buffer::Length(buffer_obj);
    ssize_t bytes_read = read(fd, buffer_data, buffer_length);
    if (bytes_read < 0) return ThrowException(ErrnoException(errno));
    // reset current pointer
    size_t seek_ret = lseek(fd,bytes_read,SEEK_CUR);
    return scope.Close(Integer::New(bytes_read));
  }






  static Handle<Value> Write(const Arguments& args) {
    HandleScope scope;
    
    if (!args[0]->IsInt32())  {
      return scope.Close(THROW_BAD_ARGS);
    }
    int fd = args[0]->Int32Value();

    if (!Buffer::HasInstance(args[1])) {
      return ThrowException(Exception::Error(String::New("Second argument needs to be a buffer")));
    }

    Local<Object> buffer_obj = args[1]->ToObject();
    char *buffer_data = Buffer::Data(buffer_obj);
    size_t buffer_length = Buffer::Length(buffer_obj);
    
    int n = write(fd, buffer_data, buffer_length);
    return scope.Close(Integer::New(n));

  }

  static Handle<Value> Close(const Arguments& args) {
    HandleScope scope;
    
    if (!args[0]->IsInt32())  {
      return scope.Close(THROW_BAD_ARGS);
    }
    int fd = args[0]->Int32Value();

    close(fd);

    return scope.Close(Integer::New(1));
  }

  static Handle<Value> Open(const Arguments& args) {
    HandleScope scope;

    //struct termios options; 

    long Baud_Rate = 38400;
    int Data_Bits = 8;
    int Stop_Bits = 1;
    int Parity = 0;
    int Flowcontrol = 0;

    long BAUD;
    long DATABITS;
    long STOPBITS;
    long PARITYON;
    long PARITY;

    if (!args[0]->IsString()) {
      return scope.Close(THROW_BAD_ARGS);
    }

    // Baud Rate Argument
    if (args.Length() >= 2 && !args[1]->IsInt32()) {
      return scope.Close(THROW_BAD_ARGS);
    } else {
      Baud_Rate = args[1]->Int32Value();
    }

    // Data Bits Argument
    if (args.Length() >= 3 && !args[2]->IsInt32()) {
      return scope.Close(THROW_BAD_ARGS);
    } else {
      Data_Bits = args[2]->Int32Value();
    }

    // Stop Bits Arguments
    if (args.Length() >= 4 && !args[3]->IsInt32()) {
      return scope.Close(THROW_BAD_ARGS);
    } else {
      Stop_Bits = args[3]->Int32Value();
    }

    // Parity Arguments
    if (args.Length() >= 5 && !args[4]->IsInt32()) {
      return scope.Close(THROW_BAD_ARGS);
    } else {
      Parity = args[4]->Int32Value();
    }

    // Flow control Arguments
    if (args.Length() >= 6 && !args[5]->IsInt32()) {
      return scope.Close(THROW_BAD_ARGS);
    } else {
      Flowcontrol = args[5]->Int32Value();
    }

    // symbol lookup documentation, DCB structure, "Defines the control setting for a serial communications device;"
    //   http://msdn.microsoft.com/en-us/library/windows/desktop/aa363189%28v=VS.85%29.aspx

    switch (Baud_Rate)
      {
      default:
        BAUD = BAUD_38400;
        break;
      case -1:
        BAUD = BAUD_USER; // baud user indeed
        break;
      case 128000:
        BAUD = BAUD_128K;
        break;
      case 115200:
        BAUD = BAUD_115200;
        break;
      case 57600:
        BAUD = BAUD_57600;
        break;
      case 56000:
        BAUD = BAUD_56K;
        break;
      case 38400:
        BAUD = BAUD_38400;
        break;
      case 19200:
        BAUD  = BAUD_19200;
        break;
      case 14400: // 14ftw
        BAUD  = BAUD_14400;
        break;
      case 9600:
        BAUD  = BAUD_9600;
        break;
      case 7200:
        BAUD  = BAUD_7200;
        break;
      case 4800:
        BAUD  = BAUD_4800;
        break;
      case 2400:
        BAUD  = BAUD_2400;
        break;
      case 1800:
        BAUD  = BAUD_1800;
        break;
      case 1200:
        BAUD  = BAUD_1200;
        break;
      case 600:
        BAUD  = BAUD_600;
        break;
      case 300:
        BAUD  = BAUD_300;
        break;
      case 150:
        BAUD  = BAUD_150;
        break;
      case 134:
        BAUD  = BAUD_134_5;
        break;
      case 110:
        BAUD  = BAUD_110;
        break;
      case 75:
        BAUD  = BAUD_075;
        break;
      }
    
    switch (Data_Bits)
      {
      default:
        DATABITS = DATABITS_8;
        break;
      case -16:
        DATABITS = DATABITS_16X;
      case 16:
	DATABITS = DATABITS_16;
      case 8:
        DATABITS = DATABITS_8;
        break;
      case 7:
        DATABITS = DATABITS_7;
        break;
      case 6:
        DATABITS = DATABITS_6;
        break;
      case 5:
        DATABITS = DATABITS_5;
        break;
      }
    switch (Stop_Bits)
      {
      case 1:
      default:
        STOPBITS = STOPBITS_10;
        break;
      case 2:
        STOPBITS = STOPBITS_20;
        break;
      case 3:
        STOPBITS = STOPBITS_15;
        break;
      } 

    switch (Parity)
      {
      case 0:
      default:
	PARITY = PARITY_NONE;
	break;
      case 1:
	PARITY = PARITY_ODD;
	break;
      case 2:
        PARITY = PARITY_EVEN;
        break;
      case 3:
        PARITY = PARITY_MARK;
        break;
      case 4:
        PARITY = PARITY_SPACE;
        break;
      }

    String::Utf8Value path(args[0]->ToString());
    
    //int flags = (O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
    //int fd    = open(*path, flags);

    // Configuring A Communication Resource;
    //   http://msdn.microsoft.com/en-us/library/windows/desktop/aa363201%28v=VS.85%29.aspx

    DCB dcb;
    HANDLE hCom;
    BOOL fSuccess;
    TCHAR *pcCommPort = TEXT("COM1");

    // TODO: marshal Utf8Value into TCHAR*
    hCom = CreateFile( pcCommPort, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL );

    if (hCom == INVALID_HANDLE_VALUE) 
    {
      return scope.Close(Integer::New(1));
    }

    SecureZeroMemory(&dcb, sizeof(DCB));
    dcb.DCBlength = sizeof(DCB);

    fSuccess = GetCommState(hCom, &dcb);
    if (!fSuccess) 
    {
      return scope.Close(Integer::New(2));
    }

    dcb.BaudRate = BAUD;
    dcb.ByteSize = DATABITS;
    dcb.Parity = PARITY;
    // dcb.fParity |= PARITY > 0 ? 1 : 0;
    dcb.StopBits = STOPBITS;

    dcb.fOutxCtsFlow = 0;
    dcb.fOutxDsrFlow = 0;
    dcb.fDtrControl = 0;
    dcb.fDsrSensitivity = 0;
    dcb.fRtsControl = 0;
    if(Flowcontrol == 1)
    {
      dcb.fOutxCtsFlow = TRUE;
      dcb.fRtsControl = CTS_CONTROL_ENABLE;
    } else if (Flowcontrol == 2) {
      dcb.fOutxDsrFlow = TRUE;
      dcb.fDtrControl = 1;
      dcb.fDsrSensitivity = 1;
    }

    fSuccess = SetCommState(hCom, &dcb);

    if (!fSuccess) 
    {
      return scope.Close(Integer::New(3));
    }

    return scope.Close(Integer::New(fd));
  }


  void SerialPort::Initialize(Handle<Object> target) {
    
    HandleScope scope;

    NODE_SET_METHOD(target, "open", Open);
    NODE_SET_METHOD(target, "write", Write);
    NODE_SET_METHOD(target, "close", Close);
    NODE_SET_METHOD(target, "read", Read);

    errno_symbol = NODE_PSYMBOL("errno");


  }


  extern "C" void
  init (Handle<Object> target) 
  {
    HandleScope scope;
    SerialPort::Initialize(target);
  }


}
