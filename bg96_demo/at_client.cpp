#include <Arduino.h>

#include "at_client.h"
#include "fifo.h"


#define DEBUG_AT_COMMANDS     1

#define UART_FIFO_SIZE                 256
Fifo_t FifoRx;
uint8_t FifoRxBuffer[UART_FIFO_SIZE];

//
// Define this low level functions based on device
//


void at_serial_init(uint32_t uart_speed)
{
  Serial1.begin(uart_speed);
  FifoInit( &FifoRx, FifoRxBuffer, UART_FIFO_SIZE );
}

bool _serial_available() 
{

  return Serial1.available();
}

char _serial_read() 
{
  char c;
  
  c = Serial1.read();
  return c;
}

void _serial_write(char c) 
{
  Serial1.write(c);
}


void _serial_print(const char *string) 
{
  int i;
  int len = strlen(string);

// Serial.println("Sending ->");
  for (i=0;i<len;i++)
  {
    _serial_write(string[i]);
    delay(1);
   // Serial.write(string[i]);
  }
//Serial.println("<-");
}

void at_serial_clear_buffers()
{
  char c;
  //Serial.println("CLEAR BUFFERS");
  while( _serial_available()) 
  {
    c=_serial_read();
   // Serial.print(c,HEX);
  }
   //Serial.println();
}




void at_send_char(char data)
{
  _serial_write(data);
}

void at_send_command(char *command ) 
{
  char command_echo[256];  
  at_serial_clear_buffers();

  sprintf(command_echo,"%s%s",command,AT_COMMAND_CR);  
  _serial_print(command_echo);
}

uint8_t at_read_for_response_single( char* response_string, uint32_t timeout_ms) 
{
  
  // Index for how many characters have been matched.
  uint8_t matched_chars = 0;
  uint8_t response_length = strlen(response_string);
  char c;
  
  // Store the start time for detecting a timeout.
  unsigned long start_time = millis();
  /*
Serial.println(response_string);
Serial.println(response_length);
Serial.println("Read For Response ->");
  */
  // Loop until the timeout is reached.
  while (millis() < (start_time + timeout_ms)) 
  {
    // Only proceed if a character is available.
    if (_serial_available()) 
    {
      c = _serial_read();

      //Serial.print(c);
      
      // If the character matches the expected character in the response,
      // increment the pointer.  If not, reset things.
      if (c == response_string[matched_chars]) 
      {
      
        matched_chars++;
       if (matched_chars == response_length) 
        {
         // Serial.println("<- SUCCESS");
          return AT_COMMAND_SUCCESS;
        }
      } 
      else 
      {
        // Character did not match - reset.
        matched_chars = 0;
      }
    }
  }

  // Timeout reached with no match found.
  //Serial.println("<- TIMEOUT");
  return AT_COMMAND_TIMEOUT;  
}

// Same as above, but with two sets of counters/indexes/etc.
uint8_t read_for_responses_dual(char* pass_string, char* fail_string, uint32_t timeout_ms) 
{
  uint8_t pass_matched_chars = 0, fail_matched_characters = 0;
  uint8_t pass_response_length = strlen(pass_string);
  uint8_t fail_response_length = strlen(fail_string);

  // Store the start time for timeout purposes.
  unsigned long start_time = millis();

//Serial.println(fail_string);
//Serial.println(fail_response_length);
//Serial.println("Read For Response Dual ->");


  // Loop until the timout is reached.
  while (millis() < (start_time + timeout_ms)) 
  {
    if (_serial_available()) 
    {
      char next_character = _serial_read();

//Serial.print(next_character);

      // Check and update the "pass" case.
      if (next_character == pass_string[pass_matched_chars]) 
      {
        pass_matched_chars++;
        
        if (pass_matched_chars == pass_response_length) 
        {
  //        Serial.println("<- SUCCESS");
          return AT_COMMAND_SUCCESS;
        }
      } 
      else 
      {
        pass_matched_chars = 0;
      }

      // Check and update the "fail" case.
      if (next_character == fail_string[fail_matched_characters]) 
      {
        fail_matched_characters++;
        
        if (fail_matched_characters == fail_response_length) 
        {
         Serial.println("<- FAILURE");
          return AT_COMMAND_FAILURE;
        }
      } 
      else 
      {
        fail_matched_characters = 0;
      }
    }
  }

//Serial.println("<- TIMEOUT");
  // Timeout reached - return timeout.
  return AT_COMMAND_TIMEOUT;  
}

uint8_t at_copy_serial_to_buffer(char *buffer, char read_until, uint16_t max_bytes,  uint32_t timeout_ms) 
{

  unsigned long start_time = millis();
  uint16_t bytes_read = 0;
//Serial.println( "COPY->");

  // Loop until timeout.
  while (millis() < (start_time + timeout_ms)) {
    if (_serial_available()) {
      buffer[bytes_read] = _serial_read();

//Serial.println( buffer );

      /**
       * Check to see if the character just read matches the read_until
       * character.  If so, stomp that character with a null terminating byte
       * and return success.
       */
      if (buffer[bytes_read] == read_until) {
        buffer[bytes_read] = 0;
//        Serial.println( "<- SUCCESS");
        return AT_COMMAND_SUCCESS;
      }

      /**
       * Increment bytes_read, and check to see if we're out of space.
       * 
       * If max_bytes is 4, offsets 0, 1, 2 can be used for characters, but
       * offset 3 must be saved for the null terminator.
       * 
       * If the length is exceeded, null terminate what has been read and return
       * the proper error.  Note that the remaining data in the serial buffer is
       * NOT read - that can be read by the calling code again, if they wish.
       */
      bytes_read++;

      if (bytes_read >= (max_bytes - 1)) {
        buffer[bytes_read] = 0;
        return AT_COMMAND_LENGTH_EXCEEDED;
      }
    }
  }

  //Serial.println( "<- TIMEOUT");

  // Timeout reached - return timeout.
  return AT_COMMAND_TIMEOUT;  
}


uint8_t at_copy_serial_to_buffer_match(char *buffer, char *match_string, uint16_t max_bytes, uint32_t timeout_ms) 
{

  char debug_string[256];
  
  unsigned long start_time = millis();
  uint16_t bytes_read = 0;

  uint8_t match_matched_chars = 0;
  uint8_t match_response_length = strlen(match_string);
  
  // Loop until timeout.
  while (millis() < (start_time + timeout_ms)) {
    if (_serial_available()) {
      buffer[bytes_read] = _serial_read();

/*
snprintf(debug_string,bytes_read,"%s",buffer);
Serial.println( debug_string );
*/
      /**
       * Check to see if the character just read matches the read_until
       * character.  If so, stomp that character with a null terminating byte
       * and return success.
       */

      // Check and update the "pass" case.
      if (buffer[bytes_read] == match_string[match_matched_chars]) 
      {
/*
        Serial.println("MATCH CHAR");
        Serial.println( match_string[match_matched_chars]);
*/
        match_matched_chars++;
        
        if (match_matched_chars == match_response_length) 
        {
          buffer[bytes_read-match_response_length-1] = 0;
          //Serial.println("<- SUCCESS");
          return AT_COMMAND_SUCCESS;
        }
      } 
      else 
      {
        match_matched_chars = 0;
      }
       

      /**
       * Increment bytes_read, and check to see if we're out of space.
       * 
       * If max_bytes is 4, offsets 0, 1, 2 can be used for characters, but
       * offset 3 must be saved for the null terminator.
       * 
       * If the length is exceeded, null terminate what has been read and return
       * the proper error.  Note that the remaining data in the serial buffer is
       * NOT read - that can be read by the calling code again, if they wish.
       */
      bytes_read++;

      if (bytes_read >= (max_bytes - 1)) {
        buffer[bytes_read] = 0;
        return AT_COMMAND_LENGTH_EXCEEDED;
      }
    }
  }

  //Serial.println( "<- TIMEOUT");

  // Timeout reached - return timeout.
  return AT_COMMAND_TIMEOUT;  
}


uint8_t at_read_until(char read_until, uint16_t timeout_ms) 
{
  unsigned long start_time = millis();

  while (millis() < (start_time + timeout_ms)) 
  {
    if (_serial_available()) 
    {
      // If the character matches the expected termination character, return.
      if (read_until == _serial_read() ) 
      {
        return AT_COMMAND_SUCCESS;
      }
    }
  }

  return AT_COMMAND_TIMEOUT;  
}

