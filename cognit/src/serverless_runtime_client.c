#include "serverless_runtime_client.h"

void init_serverless_runtime_cli(const char* c_endpoint) {
    // Implementa la lógica aquí
}

exec_response_t faas_exec_sync(uint8_t* ui8_payload, size_t payload_len) {
    exec_response_t response;
    // Implementa la lógica aquí
    return response;
}

async_exec_response_t faas_exec_async(uint8_t* ui8_payload, size_t payload_len) {
    async_exec_response_t response;
    // Implementa la lógica aquí
    return response;
}

async_exec_response_t waitForTask(const char* c_async_task_id, uint32_t ui32_timeout_ms) {
    async_exec_response_t response;
    // Implementa la lógica aquí
    return response;
}