// Penelopean Robotics Copyright (C) 2019 FoAM Kernow
// 
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

// Messages supported (all 32 bytes)
// P: Ping (flash LEDs twice)
// S: Sync servo sequence
// M: Update servo sequence
// T: Request telemetry
// H: Halt machine
// R: Reset and start machine
// W: Write to yarn machine memory
// E: Save current heap to eeprom
// C: Calibrate and return calibration data

// incoming data packets over the radio
// this is the original packet used for the motion sequencer
typedef struct {
  char pad;
  unsigned int ms_per_step;      // 2
  unsigned int length;           // 2 
  char pattern[24];              // not max pattern, which is now 12 
} seq_pattern_packet;            // = 32 bytes

typedef struct {
  char pad; // 1
  unsigned short start_address;  // 2
  unsigned short size;           // 2
  char data[25];
} bulk_write_packet;

typedef struct {
  char pad;
  unsigned short start_address; // 2
} request_telemetry_packet;

typedef struct {
  char calibrate;
  unsigned short samples; // 2
  char return_sense;
} request_calibration_packet;

typedef struct {
  char pad;
  unsigned short beat; // 2
  unsigned short ms_per_step; // 2
  unsigned short reg_1;       
  unsigned short reg_1_val;   // 4
  unsigned short reg_2;       
  unsigned short reg_2_val;   // 12
  unsigned short reg_3;       
  unsigned short reg_3_val;   // 16
  unsigned short reg_4;       
  unsigned short reg_4_val;   // 20
  unsigned short reg_5;       
  unsigned short reg_5_val;   // 24
  unsigned short reg_6;       
  unsigned short reg_6_val;   // 28
  unsigned short reg_7;       
  unsigned short reg_7_val;   // 32
} sync_packet;
