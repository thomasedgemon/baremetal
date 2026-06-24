#include <stdint.h>  // Gives us fixed-size integer types like uint32_t.

// Blink five external LEDs on and off randomly from a C++ bare-metal program.

// Number of external LEDs in this project.
// static: keep this name local to this file.
// constexpr: this value is known at compile time and cannot change while running.
// uint32_t: an unsigned 32-bit integer.
static constexpr uint32_t LED_COUNT = 5;

// Function format in C/C++:
//
//     return_type function_name(parameter_type parameter_name, ...);
//
// Examples:
//     void led_on(uint32_t index);
//     uint32_t random_u32();
//
// void means the function does not return a value.
// uint32_t means the function returns an unsigned 32-bit integer.
// Parameters are input values passed into the function.
// A line ending in ; is a declaration: it tells the compiler the function exists.
// A function with { ... } is a definition: it contains the code that actually runs.

// MMIO = memory-mapped input/output: hardware registers are controlled by
// reading and writing normal-looking memory addresses.
static inline void mmio_write(uint32_t address, uint32_t value);
static inline uint32_t mmio_read(uint32_t address);

// Base addresses are the starting addresses of major RP2350 hardware blocks.
// A specific register is found by adding its offset to the block base address.
static constexpr uint32_t RESETS_BASE     = 0x40020000;  // Reset controller block.
static constexpr uint32_t IO_BANK0_BASE   = 0x40028000;  // GPIO function/select/status block.
static constexpr uint32_t PADS_BANK0_BASE = 0x40038000;  // Electrical pad settings for GPIO pins.
static constexpr uint32_t SIO_BASE        = 0xd0000000;  // Single-cycle IO block for fast GPIO control.
static constexpr uint32_t TIMER0_BASE     = 0x400b0000;  // Timer block, useful later for delays/random seed.

// RESETS register addresses.
// RESET holds peripherals in reset when a bit is 1.
// RESET_DONE reports that a peripheral has finished coming out of reset.
static constexpr uint32_t RESETS_RESET      = RESETS_BASE + 0x000;
static constexpr uint32_t RESETS_RESET_DONE = RESETS_BASE + 0x008;

// RESETS bits needed before GPIO can work.
// 1u means unsigned 1. Shifting it left chooses the bit for that hardware block.
static constexpr uint32_t RESET_IO_BANK0   = 1u << 6;  // Bit 6 controls IO_BANK0 reset.
static constexpr uint32_t RESET_PADS_BANK0 = 1u << 9;  // Bit 9 controls PADS_BANK0 reset.

// SIO GPIO register addresses for GPIO0 through GPIO31.
// These let your code set, clear, toggle, and enable GPIO output bits.
static constexpr uint32_t SIO_GPIO_OUT     = SIO_BASE + 0x010;  // Current output values for GPIO pins.
static constexpr uint32_t SIO_GPIO_OUT_SET = SIO_BASE + 0x018;  // Write 1 bits here to turn GPIO outputs on.
static constexpr uint32_t SIO_GPIO_OUT_CLR = SIO_BASE + 0x020;  // Write 1 bits here to turn GPIO outputs off.
static constexpr uint32_t SIO_GPIO_OUT_XOR = SIO_BASE + 0x028;  // Write 1 bits here to toggle GPIO outputs.
static constexpr uint32_t SIO_GPIO_OE_SET  = SIO_BASE + 0x038;  // Write 1 bits here to make pins outputs.
static constexpr uint32_t SIO_GPIO_OE_CLR  = SIO_BASE + 0x040;  // Write 1 bits here to make pins inputs.

// Return the IO_BANK0 CTRL register address for one GPIO pin.
// CTRL is where you choose the pin function, such as SIO for direct GPIO control.
// This is a small function definition because it has a body between { and }.
static constexpr uint32_t gpio_ctrl_addr(uint32_t gpio) {
    // return sends this calculated value back to whoever called gpio_ctrl_addr().
    // GPIO0 CTRL is base + 0x004. Each GPIO then takes another 0x008 bytes.
    return IO_BANK0_BASE + 0x004 + gpio * 0x008;
}

// Return the PADS_BANK0 register address for one GPIO pin.
// The pad register controls electrical behavior like output disable and drive strength.
static constexpr uint32_t gpio_pad_addr(uint32_t gpio) {
    // GPIO0 pad is base + 0x004. Each GPIO pad register then takes 0x004 bytes.
    return PADS_BANK0_BASE + 0x004 + gpio * 0x004;
}

// Function declarations for the pieces you will implement in later tasks.
// These are listed before main() so main() is allowed to call them.
void gpio_init_output(uint32_t gpio);  // Configure one GPIO pin as an output.
void led_on(uint32_t index);           // Turn on one LED by index, 0 through 4.
void led_off(uint32_t index);          // Turn off one LED by index, 0 through 4.
void led_toggle(uint32_t index);       // Flip one LED from on to off or off to on.
void delay_ms(uint32_t ms);            // Wait for approximately the requested milliseconds.
void init_gpio_hardware();             // Bring GPIO-related hardware blocks out of reset.
uint32_t random_u32();                 // Return a pseudo-random 32-bit number.

// This array maps LED indexes to physical GPIO numbers.
// Example: led_pins[0] is GPIO2, so LED index 0 is wired to GPIO2.
// Arrays use square brackets. led_pins[3] means element number 3.
// C++ arrays are zero-based, so this array has indexes 0, 1, 2, 3, and 4.
static constexpr uint32_t led_pins[LED_COUNT] = {
    2,  // LED 0 -> GPIO2
    3,  // LED 1 -> GPIO3
    4,  // LED 2 -> GPIO4
    5,  // LED 3 -> GPIO5
    6   // LED 4 -> GPIO6
};

// main() is the entry point: it is where your program starts running.
// In bare-metal firmware, main() usually never returns because there is no OS
// waiting for the program to finish.
int main() {
    // A function call uses the function name followed by parentheses.
    // This calls init_gpio_hardware(), which you will define later.
    init_gpio_hardware();

    // for-loop format:
    //
    //     for (start_value; keep_looping_while_this_is_true; update_after_each_loop) {
    //         code to repeat
    //     }
    //
    // Here, i starts at 0, loops while i < LED_COUNT, and ++i adds 1 each time.
    for (uint32_t i = 0; i < LED_COUNT; ++i) {
        // led_pins[i] looks up the GPIO number for LED index i.
        gpio_init_output(led_pins[i]);
    }

    // Start with all LEDs off so the program begins from a known state.
    for (uint32_t i = 0; i < LED_COUNT; ++i) {
        // led_off() expects an LED index, not a GPIO number.
        led_off(i);
    }

    // while-loop format:
    //
    //     while (condition) {
    //         code to repeat
    //     }
    //
    // true is always true, so while (true) repeats forever.
    while (true) {
        // % is the modulo operator. It gives the remainder after division.
        // random_u32() % LED_COUNT converts a large random number into 0..4.
        uint32_t led_index = random_u32() % LED_COUNT;

        // Pick a random action:
        // 0 = off, 1 = on, 2 = toggle.
        uint32_t action = random_u32() % 3;

        // if / else if / else chooses one code path.
        // == compares two values. It is different from =, which assigns a value.
        if (action == 0) {
            led_off(led_index);
        } else if (action == 1) {
            led_on(led_index);
        } else {
            led_toggle(led_index);
        }

        // This makes a delay from 50 to 500 milliseconds:
        // random_u32() % 451 gives 0..450, then adding 50 gives 50..500.
        uint32_t delay = 50 + (random_u32() % 451);
        delay_ms(delay);
    }
}
