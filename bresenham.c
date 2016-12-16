#define N_LEDS 16
#define N_STEPS 100

uint8_t  target_val[N_LEDS] = {0, 0, 0, 0, 0, 15, 15, 15, 15, 15, 15, 255, 255, 255, 255, 255}, current_val[N_LEDS] = {0};
uint32_t target_sum[N_LEDS] = {0}, current_sum[N_LEDS] = {0};
uint32_t step = 0;
uint16_t LED_word = 0;

void SPI1_IRQHandler(void)
{
	SPI1->CR2 &= ~SPI_CR2_TXEIE;
	TIM6->CR1 |= TIM_CR1_CEN;
	
	if(step++ == N_STEPS) {
		step = 0;
		for(uint8_t led = 0; led < N_LEDS; led++)
			 target_sum[led] = current_sum[led] = 0;
	}
	LED_word = 0;
	for(uint8_t led = 0; led < N_LEDS; led++) {
		current_val[led]  = (target_sum[led] >= current_sum[led]) ? 1 : 0;
		 target_sum[led] +=  target_val[(N_LEDS + 11 - led) % N_LEDS];
		current_sum[led] += current_val[led] * N_STEPS;

		LED_word *= 2;
		LED_word |= current_val[led];
	}
}

void TIM6_IRQHandler(void)
{
	GPIOA->BSRR = GPIO_PIN_4;
	TIM6->SR   &= ~TIM_SR_UIF;
	SPI1->DR    = LED_word;
	SPI1->CR2   = SPI_CR2_TXEIE;
	GPIOA->BRR  = GPIO_PIN_4;
}
