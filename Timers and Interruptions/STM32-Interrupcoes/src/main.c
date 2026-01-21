#include "stm32f4xx_hal.h"

TIM_HandleTypeDef htim2;

void SysTick_Handler(void){
    HAL_IncTick();
}

void Configurar_Timer(void){
    __HAL_RCC_TIM2_CLK_ENABLE();

    //Qual timer usar
    htim2.Instance = TIM2;

    htim2.Init.Prescaler = 15999;
    htim2.Init.Period = 499;

    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    HAL_TIM_Base_Init(&htim2);
}

void EXTI15_10_IRQHandler(void){
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
}

void TIM2_IRQHandler(void){
    HAL_TIM_IRQHandler(&htim2);

}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

    if(GPIO_Pin == GPIO_PIN_13){
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

        //zerar timer
        __HAL_TIM_SET_COUNTER(&htim2, 0);

        HAL_TIM_Base_Start_IT(&htim2);
    
        
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
    if(htim->Instance == TIM2){
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

        HAL_TIM_Base_Stop_IT(&htim2);
    }
}

int main(void){

    HAL_Init(); // Acorda o sistema e inicializa o Systick
    //clock e gpio
    __HAL_RCC_GPIOA_CLK_ENABLE(); //ligando a energia na porta A
    __HAL_RCC_GPIOC_CLK_ENABLE();
    
    Configurar_Timer();
    GPIO_InitTypeDef cfg = {0}; //Iniciar toda a struct em zero

    cfg.Pin = GPIO_PIN_5;
    cfg.Mode = GPIO_MODE_OUTPUT_PP;
    cfg.Pull = GPIO_NOPULL;
    cfg.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &cfg);

    cfg.Pin = GPIO_PIN_13;
    cfg.Mode = GPIO_MODE_IT_FALLING;
    cfg.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &cfg);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0); //Configuração NVIC com prioridade 0
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0); //Prioridade menor para não ferrar com o botão
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    while(1){

       //Processador só acordar com interrupções e etc 
      HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
    }

}