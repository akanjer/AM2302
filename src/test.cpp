#include <Arduino.h>
#include <stdint.h>

const uint8_t PIN_LED = PB5;
const uint8_t PIN_SENSOR = PD2;

uint8_t sensorData[5] = {0};

void setup()
{
    Serial.begin(115200);

    // Set PIN_LED output
    DDRB |= _BV(PIN_LED);
}

void loop()
{
    // Set sensor pin as output
    DDRD |= _BV(PIN_SENSOR);

    // Pull sensor pin up
    PORTD |= _BV(PIN_SENSOR);

    // Blink some bling
    PORTB |= _BV(PIN_LED);
    delay(1000);
    PORTB &= ~_BV(PIN_LED);
    delay(1000);
    PORTB |= _BV(PIN_LED);
    delay(1000);

    // Pull sensor low to signal start
    PORTD &= ~_BV(PIN_SENSOR);

    // Wait 10 us for sensor to recognize start
    _delay_us(10);

    // Now pull up
    PORTD |= _BV(PIN_SENSOR);

    // And wait for response
    _delay_us(30);

    // Set as input
    DDRD &= ~_BV(PIN_SENSOR);

    // wait until pin becomes low
    while (PIND & _BV(PIN_SENSOR))
    {
    }

    // LOW

    // wait until becomes high
    while (!(PIND & _BV(PIN_SENSOR)))
    {
    }

    // HIGH

    // Start transmission
    for (uint8_t i = 0; i < 40; i++)
    {
        // Wait until it becomes low. This applies to first and subsequent iterations in case
        // bit check was high
        while (PIND & _BV(PIN_SENSOR))
        {
        }

        // LOW, it's a start transmission signal

        // Wait until pin becomes high
        while (!(PIND & _BV(PIN_SENSOR)))
        {
        }

        // HIGH, this is where data bit is sent. Datasheet specifies HIGH for 26-28 us is 0 and 1 is 70 us.
        // If we check at 40 us and pin is high it means bit is 1, and it it's low it means
        // we're in next iteration roughly 10 us into the trasmission start signal.

        _delay_us(40);

        sensorData[i/8] <<= 1;

        if (PIND & _BV(PIN_SENSOR))
        {
            sensorData[i/8] |= 1;
        }
    }

    // Checksum check
    if (sensorData[4] == ((sensorData[0] + sensorData[1] + sensorData[2] + sensorData[3]) & 0xFF))
    {
        Serial.println("Checksum OK");
    }
    else
    {
        Serial.println("Checksum OK");
    }

    // Print the data
    float f = sensorData[2] & 0x7F;
    f *= 256;
    f += sensorData[3];
    f *= 0.1;

    if (sensorData[2] & 0x80)
    {
        f *= -1;
    }

     Serial.println("Temperatue:" + String(f));

     // Print the data
     f = sensorData[0];
     f *= 256;
     f += sensorData[1];
     f *= 0.1;

     Serial.println("Humidity:" + String(f));

}
