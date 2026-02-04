#include "stm32f4xx_hal.h"
#include <string.h>
#define MAX_BUFFER 50
uint8_t rx_data;
uint8_t main_buffer[MAX_BUFFER];
int buffer_index = 0;

UART_HandleTypeDef huart2;



void Simple_UART_Init(void) {
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&huart2);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
    if (huart->Instance == USART2){

        if (rx_data == '\r' || rx_data == '\n') {
            
            if (buffer_index > 0) {
                main_buffer[buffer_index] = '\0';
                
                if (strcmp((char*)main_buffer, "ligar") == 0) {
                    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n-> ACENDEU\r\n", 14, 100);
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
                } 
                else if (strcmp((char*)main_buffer, "desliga") == 0) {
                    HAL_UART_Transmit(&huart2, (uint8_t*)"\r\n-> APAGOU\r\n", 13, 100);
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
                } 
                else {
                    char msg[60];
                    sprintf(msg, "\r\nErro: [%s] desconhecido\r\n", main_buffer);
                    HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), 100);
                }
                buffer_index = 0; // Reseta o índice
            }
        } 
        else {
            //filtro + acumulação
            if (rx_data >= 32 && buffer_index < MAX_BUFFER - 1) {
                
                main_buffer[buffer_index++] = rx_data;
                
            }
        }
        HAL_UART_Receive_IT(&huart2, &rx_data, 1);

    }

}

void USART2_IRQHandler(void) {
    HAL_UART_IRQHandler(&huart2);
}

int main(void) {
    HAL_Init();
    Simple_UART_Init();
    HAL_UART_Receive_IT(&huart2, &rx_data, 1);

    // Configura o LED verde da Nucleo (PA5)
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_Led = {0};
    GPIO_Led.Pin = GPIO_PIN_5;
    GPIO_Led.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_Led);


   while (1) {

}
}

// Necessário para não dar erro de compilação
void SysTick_Handler(void) { HAL_IncTick(); }