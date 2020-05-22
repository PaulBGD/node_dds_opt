#include <napi.h>
#include <iostream>
#include <filesystem>
#include "textures.hpp"

class Optimizer : public Napi::ObjectWrap<Optimizer> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    Optimizer(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    TexturesOptimizer _optimizer;

    Napi::Value Open(const Napi::CallbackInfo &info);

    Napi::Value Info(const Napi::CallbackInfo &info);

    Napi::Value DoCPUWork(const Napi::CallbackInfo &info);

    Napi::Value DoGPUWork(const Napi::CallbackInfo &info);

    Napi::Value Save(const Napi::CallbackInfo &info);
};

Optimizer::Optimizer(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Optimizer>(info) {}

Napi::Object Optimizer::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "Optimizer", {
            InstanceMethod<&Optimizer::Open>("open"),
            InstanceMethod<&Optimizer::Info>("info"),
            InstanceMethod<&Optimizer::DoCPUWork>("doCPUWork"),
            InstanceMethod<&Optimizer::DoGPUWork>("doGPUWork"),
            InstanceMethod<&Optimizer::Save>("save")
    });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();
    exports.Set("Optimizer", func);
    return exports;
}

Napi::Value Optimizer::Open(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Expected string").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string file = info[0].As<Napi::String>().ToString();
    std::filesystem::path filePath = std::filesystem::absolute(file);

    if (!_optimizer.open(filePath.string(), TexturesOptimizer::TextureType::DDS)) {
        Napi::TypeError::New(env, "failed to open file").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, true);
}

Napi::Value Optimizer::Info(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    Napi::Object data = Napi::Object::New(env);
    data.Set("width", Napi::Number::New(env, _optimizer.getInfo().width));
    data.Set("height", Napi::Number::New(env, _optimizer.getInfo().height));
    return data;
}

Napi::Value Optimizer::DoCPUWork(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() != 2) {
        Napi::TypeError::New(env, "Wrong number of arguments, expected width and height").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsNumber() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Width and height must be numbers").ThrowAsJavaScriptException();
        return env.Null();
    }
    uint32_t width = info[0].ToNumber().Uint32Value();
    uint32_t height = info[1].ToNumber().Uint32Value();
    if (!_optimizer.doCPUWork(width, height)) {
        Napi::TypeError::New(env, "Failed to do CPU work").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, true);
}

Napi::Value Optimizer::DoGPUWork(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments, expected gpu number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "GPU number must be.. number").ThrowAsJavaScriptException();
        return env.Null();
    }
    uint32_t gpuNum = info[0].ToNumber().Uint32Value();
    if (!_optimizer.doGPUWork(gpuNum)) {
        Napi::TypeError::New(env, "Failed to do GPU work").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, true);
}

Napi::Value Optimizer::Save(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    if (info.Length() != 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Expected string").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string file = info[0].As<Napi::String>().ToString();
    std::filesystem::path filePath = std::filesystem::absolute(file);

    if (!_optimizer.saveToFile(filePath.string())) {
        Napi::TypeError::New(env, "Failed to save file").ThrowAsJavaScriptException();
        return env.Null();
    }
    return Napi::Boolean::New(env, true);
}

Napi::FunctionReference Optimizer::constructor;

static Napi::Object Init(Napi::Env env, Napi::Object exports) {
    Optimizer::Init(env, exports);
    return exports;
}

NODE_API_MODULE(node_dds_opt, Init)
