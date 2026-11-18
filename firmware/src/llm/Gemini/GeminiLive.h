#if defined(REALTIME_API)

#ifndef _GEMINI_LIVE_H
#define _GEMINI_LIVE_H

#include <Arduino.h>
#include <M5Unified.h>
#include "StackchanExConfig.h"
#include "SpiRamJsonDocument.h"
#include "../ChatHistory.h"
#include "../RealtimeLLMBase.h"
#include "../ChatGPT/FunctionCall.h"
#include <WebSocketsClient.h>
#include <atomic>


class GeminiLive: public RealtimeLLMBase{
private:
    // WebSocketタスクからUIタスクへ渡す。-1=未選択、0=通常、1=笑顔。
    std::atomic<int> responseExpression{-1};

public:   //本当はprivateにしたいところだがコールバック関数にthisポインタを渡して使うためにpublicとした
    MCPClient* mcpClient[LLM_N_MCP_SERVERS_MAX];
    FunctionCall* fnCall;
    
    String role;
    String userInfo;
    String systemRole;

public:
    GeminiLive(llm_param_t param);

    virtual void chat(String text, const char *base64_buf = NULL) {};   //dummy
    virtual String& buildInputAudioJson(String& jsonBuf, String& base64);
    virtual void load_role();
    void clearResponseExpression() override;
    bool applyResponseExpression() override;
    void handleToolCalls(JsonArrayConst calls);
};


#endif  //_GEMINI_LIVE_H

#endif  //REALTIME_API
