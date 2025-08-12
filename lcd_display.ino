#include <Wire.h> 

#include "Adafruit_MCP9601.h" 

 

// === TCA9548A Multiplexer === 

#define TCA_ADDR 0x70 

 

void selectTCA(uint8_t channel) { 

  if (channel > 7) return; 

  Wire.beginTransmission(TCA_ADDR); 

  Wire.write(1 << channel); 

  Wire.endTransmission(); 

  delay(100);  // Allow bus to stabilize 

} 

 

// === INA228 Configuration === 

#define NUM_INA 3 

const uint8_t ina_addrs[NUM_INA] = {0x41, 0x44, 0x45}; 

#define REG_SHUNT_CAL  0x03 

#define REG_VBUS       0x05 

#define REG_CURRENT    0x07 

#define SHUNT_RESISTANCE 0.0002 

#define CURRENT_LSB      0.0001 

#define VBUS_SCALE       62.0  // Calibrated multiplier 

 

void writeINARegister16(uint8_t addr, uint8_t reg, uint16_t value) { 

  Wire.beginTransmission(addr); 

  Wire.write(reg); 

  Wire.write(value >> 8); 

  Wire.write(value & 0xFF); 

  Wire.endTransmission(); 

} 

 

int32_t readINARegister24(uint8_t addr, uint8_t reg) { 

  Wire.beginTransmission(addr); 

  Wire.write(reg); 

  Wire.endTransmission(false); 

  Wire.requestFrom(addr, (uint8_t)3); 

  if (Wire.available() < 3) return 0; 

  int32_t value = 0; 

  value |= ((uint32_t)Wire.read()) << 16; 

  value |= ((uint16_t)Wire.read()) << 8; 

  value |= Wire.read(); 

  return value; 

} 

 

// === MCP9601 Configuration === 

#define MCP_ADDR 0x67 

Adafruit_MCP9601 mcp; 

 

void setup() { 

  Serial.begin(115200); 

  Wire.begin(); 

  Wire.setClock(100000); 

 

  Serial.println("🔧 Starting Fuel Cell Monitoring System..."); 

 

  // --- Initialize INA228s via TCA channel 0 

  selectTCA(0); 

  for (int i = 0; i < NUM_INA; i++) { 

    Wire.beginTransmission(ina_addrs[i]); 

    if (Wire.endTransmission() == 0) { 

      writeINARegister16(ina_addrs[i], REG_SHUNT_CAL, 8192); 

      Serial.print("✅ INA228 @ 0x"); 

      Serial.print(ina_addrs[i], HEX); 

      Serial.println(" initialized."); 

    } else { 

      Serial.print("❌ INA228 @ 0x"); 

      Serial.print(ina_addrs[i], HEX); 

      Serial.println(" not responding."); 

    } 

  } 

 

  // --- Initialize MCP9601 on main I2C bus 

  Serial.print("🔧 Initializing MCP9601 @ 0x"); 

  Serial.print(MCP_ADDR, HEX); 

  Serial.println("..."); 

  if (!mcp.begin(MCP_ADDR)) { 

    Serial.println("❌ MCP9601 not found. Check wiring!"); 

    while (1); 

  } 

  mcp.setThermocoupleType(MCP9600_TYPE_K); 

  mcp.setAmbientResolution(RES_ZERO_POINT_0625); 

  mcp.setADCresolution(MCP9600_ADCRESOLUTION_18); 

  mcp.setFilterCoefficient(3); 

  mcp.enable(true); 

  Serial.println("✅ MCP9601 ready."); 

} 

 

void loop() { 

  delay(5000);  // One reading every 5 seconds 

  Serial.println("\n------ FUEL CELL SYSTEM SNAPSHOT ------"); 

 

  // --- Read 3 INA228s via TCA 

  selectTCA(0); 

  for (int i = 0; i < NUM_INA; i++) { 

    Serial.print("INA "); Serial.print(i + 1); Serial.print(" (0x"); 

    Serial.print(ina_addrs[i], HEX); Serial.println("):"); 

 

    int32_t rawV = readINARegister24(ina_addrs[i], REG_VBUS); 

    float voltage = rawV * 0.1953125 / 1e6 * VBUS_SCALE; 

 

    int32_t rawI = readINARegister24(ina_addrs[i], REG_CURRENT); 

    float current = rawI * CURRENT_LSB; 

 

    float power = voltage * current; 

 

    Serial.print("  Voltage (V): "); Serial.println(voltage, 3); 

    Serial.print("  Current (A): "); Serial.println(current, 3); 

    Serial.print("  Power   (W): "); Serial.println(power, 3); 

  } 

 

  // --- Read MCP9601 temperature 

  Serial.print("MCP9601 Temp (°C): "); 

  float temp = mcp.readThermocouple(); 

  Serial.println(temp, 2); 

 

  Serial.println("----------------------------------------"); 

}
