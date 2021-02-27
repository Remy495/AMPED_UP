// automatically generated by the FlatBuffers compiler, do not modify


#ifndef FLATBUFFERS_GENERATED_AMPEDUPMESSAGING_AMPEDUPMESSAGING_H_
#define FLATBUFFERS_GENERATED_AMPEDUPMESSAGING_AMPEDUPMESSAGING_H_

#include "flatbuffers/flatbuffers.h"

#include "AmpedUpNodes_generated.h"
#include "AmpedUpPresets_generated.h"

namespace AmpedUpMessaging {

struct SetKnobValuesMessage;
struct SetKnobValuesMessageBuilder;

struct SavePresetMessage;
struct SavePresetMessageBuilder;

struct SaveCurrentAsPresetMessage;
struct SaveCurrentAsPresetMessageBuilder;

struct LoadPresetMessage;
struct LoadPresetMessageBuilder;

struct LogMessage;
struct LogMessageBuilder;

struct Message;
struct MessageBuilder;

enum class LogSeverity : uint8_t {
  DEBUG = 0,
  WARNING = 1,
  CRITICAL = 2,
  FATAL = 3,
  MIN = DEBUG,
  MAX = FATAL
};

inline const LogSeverity (&EnumValuesLogSeverity())[4] {
  static const LogSeverity values[] = {
    LogSeverity::DEBUG,
    LogSeverity::WARNING,
    LogSeverity::CRITICAL,
    LogSeverity::FATAL
  };
  return values;
}

inline const char * const *EnumNamesLogSeverity() {
  static const char * const names[5] = {
    "DEBUG",
    "WARNING",
    "CRITICAL",
    "FATAL",
    nullptr
  };
  return names;
}

inline const char *EnumNameLogSeverity(LogSeverity e) {
  if (flatbuffers::IsOutRange(e, LogSeverity::DEBUG, LogSeverity::FATAL)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesLogSeverity()[index];
}

enum class MessagePayload : uint8_t {
  NONE = 0,
  SetKnobValuesMessage = 1,
  SavePresetMessage = 2,
  SaveCurrentAsPresetMessage = 3,
  LoadPresetMessage = 4,
  LogMessage = 5,
  MIN = NONE,
  MAX = LogMessage
};

inline const MessagePayload (&EnumValuesMessagePayload())[6] {
  static const MessagePayload values[] = {
    MessagePayload::NONE,
    MessagePayload::SetKnobValuesMessage,
    MessagePayload::SavePresetMessage,
    MessagePayload::SaveCurrentAsPresetMessage,
    MessagePayload::LoadPresetMessage,
    MessagePayload::LogMessage
  };
  return values;
}

inline const char * const *EnumNamesMessagePayload() {
  static const char * const names[7] = {
    "NONE",
    "SetKnobValuesMessage",
    "SavePresetMessage",
    "SaveCurrentAsPresetMessage",
    "LoadPresetMessage",
    "LogMessage",
    nullptr
  };
  return names;
}

inline const char *EnumNameMessagePayload(MessagePayload e) {
  if (flatbuffers::IsOutRange(e, MessagePayload::NONE, MessagePayload::LogMessage)) return "";
  const size_t index = static_cast<size_t>(e);
  return EnumNamesMessagePayload()[index];
}

template<typename T> struct MessagePayloadTraits {
  static const MessagePayload enum_value = MessagePayload::NONE;
};

template<> struct MessagePayloadTraits<AmpedUpMessaging::SetKnobValuesMessage> {
  static const MessagePayload enum_value = MessagePayload::SetKnobValuesMessage;
};

template<> struct MessagePayloadTraits<AmpedUpMessaging::SavePresetMessage> {
  static const MessagePayload enum_value = MessagePayload::SavePresetMessage;
};

template<> struct MessagePayloadTraits<AmpedUpMessaging::SaveCurrentAsPresetMessage> {
  static const MessagePayload enum_value = MessagePayload::SaveCurrentAsPresetMessage;
};

template<> struct MessagePayloadTraits<AmpedUpMessaging::LoadPresetMessage> {
  static const MessagePayload enum_value = MessagePayload::LoadPresetMessage;
};

template<> struct MessagePayloadTraits<AmpedUpMessaging::LogMessage> {
  static const MessagePayload enum_value = MessagePayload::LogMessage;
};

bool VerifyMessagePayload(flatbuffers::Verifier &verifier, const void *obj, MessagePayload type);
bool VerifyMessagePayloadVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types);

struct SetKnobValuesMessage FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef SetKnobValuesMessageBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_KNOBVALUES_TYPE = 4,
    VT_KNOBVALUES = 6
  };
  AmpedUpPresets::KnobValues knobValues_type() const {
    return static_cast<AmpedUpPresets::KnobValues>(GetField<uint8_t>(VT_KNOBVALUES_TYPE, 0));
  }
  const void *knobValues() const {
    return GetPointer<const void *>(VT_KNOBVALUES);
  }
  template<typename T> const T *knobValues_as() const;
  const AmpedUpPresets::FixedKnobValues *knobValues_as_FixedKnobValues() const {
    return knobValues_type() == AmpedUpPresets::KnobValues::FixedKnobValues ? static_cast<const AmpedUpPresets::FixedKnobValues *>(knobValues()) : nullptr;
  }
  const AmpedUpNodes::NodeGraph *knobValues_as_AmpedUpNodes_NodeGraph() const {
    return knobValues_type() == AmpedUpPresets::KnobValues::AmpedUpNodes_NodeGraph ? static_cast<const AmpedUpNodes::NodeGraph *>(knobValues()) : nullptr;
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_KNOBVALUES_TYPE) &&
           VerifyOffset(verifier, VT_KNOBVALUES) &&
           VerifyKnobValues(verifier, knobValues(), knobValues_type()) &&
           verifier.EndTable();
  }
};

template<> inline const AmpedUpPresets::FixedKnobValues *SetKnobValuesMessage::knobValues_as<AmpedUpPresets::FixedKnobValues>() const {
  return knobValues_as_FixedKnobValues();
}

template<> inline const AmpedUpNodes::NodeGraph *SetKnobValuesMessage::knobValues_as<AmpedUpNodes::NodeGraph>() const {
  return knobValues_as_AmpedUpNodes_NodeGraph();
}

struct SetKnobValuesMessageBuilder {
  typedef SetKnobValuesMessage Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_knobValues_type(AmpedUpPresets::KnobValues knobValues_type) {
    fbb_.AddElement<uint8_t>(SetKnobValuesMessage::VT_KNOBVALUES_TYPE, static_cast<uint8_t>(knobValues_type), 0);
  }
  void add_knobValues(flatbuffers::Offset<void> knobValues) {
    fbb_.AddOffset(SetKnobValuesMessage::VT_KNOBVALUES, knobValues);
  }
  explicit SetKnobValuesMessageBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<SetKnobValuesMessage> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<SetKnobValuesMessage>(end);
    return o;
  }
};

inline flatbuffers::Offset<SetKnobValuesMessage> CreateSetKnobValuesMessage(
    flatbuffers::FlatBufferBuilder &_fbb,
    AmpedUpPresets::KnobValues knobValues_type = AmpedUpPresets::KnobValues::NONE,
    flatbuffers::Offset<void> knobValues = 0) {
  SetKnobValuesMessageBuilder builder_(_fbb);
  builder_.add_knobValues(knobValues);
  builder_.add_knobValues_type(knobValues_type);
  return builder_.Finish();
}

struct SavePresetMessage FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef SavePresetMessageBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_PRESET = 4
  };
  const AmpedUpPresets::Preset *preset() const {
    return GetPointer<const AmpedUpPresets::Preset *>(VT_PRESET);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyOffset(verifier, VT_PRESET) &&
           verifier.VerifyTable(preset()) &&
           verifier.EndTable();
  }
};

struct SavePresetMessageBuilder {
  typedef SavePresetMessage Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_preset(flatbuffers::Offset<AmpedUpPresets::Preset> preset) {
    fbb_.AddOffset(SavePresetMessage::VT_PRESET, preset);
  }
  explicit SavePresetMessageBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<SavePresetMessage> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<SavePresetMessage>(end);
    return o;
  }
};

inline flatbuffers::Offset<SavePresetMessage> CreateSavePresetMessage(
    flatbuffers::FlatBufferBuilder &_fbb,
    flatbuffers::Offset<AmpedUpPresets::Preset> preset = 0) {
  SavePresetMessageBuilder builder_(_fbb);
  builder_.add_preset(preset);
  return builder_.Finish();
}

struct SaveCurrentAsPresetMessage FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef SaveCurrentAsPresetMessageBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4
  };
  uint16_t id() const {
    return GetField<uint16_t>(VT_ID, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint16_t>(verifier, VT_ID) &&
           verifier.EndTable();
  }
};

struct SaveCurrentAsPresetMessageBuilder {
  typedef SaveCurrentAsPresetMessage Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(uint16_t id) {
    fbb_.AddElement<uint16_t>(SaveCurrentAsPresetMessage::VT_ID, id, 0);
  }
  explicit SaveCurrentAsPresetMessageBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<SaveCurrentAsPresetMessage> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<SaveCurrentAsPresetMessage>(end);
    return o;
  }
};

inline flatbuffers::Offset<SaveCurrentAsPresetMessage> CreateSaveCurrentAsPresetMessage(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint16_t id = 0) {
  SaveCurrentAsPresetMessageBuilder builder_(_fbb);
  builder_.add_id(id);
  return builder_.Finish();
}

struct LoadPresetMessage FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef LoadPresetMessageBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_ID = 4
  };
  uint16_t id() const {
    return GetField<uint16_t>(VT_ID, 0);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint16_t>(verifier, VT_ID) &&
           verifier.EndTable();
  }
};

struct LoadPresetMessageBuilder {
  typedef LoadPresetMessage Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_id(uint16_t id) {
    fbb_.AddElement<uint16_t>(LoadPresetMessage::VT_ID, id, 0);
  }
  explicit LoadPresetMessageBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<LoadPresetMessage> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<LoadPresetMessage>(end);
    return o;
  }
};

inline flatbuffers::Offset<LoadPresetMessage> CreateLoadPresetMessage(
    flatbuffers::FlatBufferBuilder &_fbb,
    uint16_t id = 0) {
  LoadPresetMessageBuilder builder_(_fbb);
  builder_.add_id(id);
  return builder_.Finish();
}

struct LogMessage FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef LogMessageBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_SEVERITY = 4,
    VT_MESSAGE = 6
  };
  AmpedUpMessaging::LogSeverity severity() const {
    return static_cast<AmpedUpMessaging::LogSeverity>(GetField<uint8_t>(VT_SEVERITY, 0));
  }
  const flatbuffers::String *message() const {
    return GetPointer<const flatbuffers::String *>(VT_MESSAGE);
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_SEVERITY) &&
           VerifyOffset(verifier, VT_MESSAGE) &&
           verifier.VerifyString(message()) &&
           verifier.EndTable();
  }
};

struct LogMessageBuilder {
  typedef LogMessage Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_severity(AmpedUpMessaging::LogSeverity severity) {
    fbb_.AddElement<uint8_t>(LogMessage::VT_SEVERITY, static_cast<uint8_t>(severity), 0);
  }
  void add_message(flatbuffers::Offset<flatbuffers::String> message) {
    fbb_.AddOffset(LogMessage::VT_MESSAGE, message);
  }
  explicit LogMessageBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<LogMessage> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<LogMessage>(end);
    return o;
  }
};

inline flatbuffers::Offset<LogMessage> CreateLogMessage(
    flatbuffers::FlatBufferBuilder &_fbb,
    AmpedUpMessaging::LogSeverity severity = AmpedUpMessaging::LogSeverity::DEBUG,
    flatbuffers::Offset<flatbuffers::String> message = 0) {
  LogMessageBuilder builder_(_fbb);
  builder_.add_message(message);
  builder_.add_severity(severity);
  return builder_.Finish();
}

inline flatbuffers::Offset<LogMessage> CreateLogMessageDirect(
    flatbuffers::FlatBufferBuilder &_fbb,
    AmpedUpMessaging::LogSeverity severity = AmpedUpMessaging::LogSeverity::DEBUG,
    const char *message = nullptr) {
  auto message__ = message ? _fbb.CreateString(message) : 0;
  return AmpedUpMessaging::CreateLogMessage(
      _fbb,
      severity,
      message__);
}

struct Message FLATBUFFERS_FINAL_CLASS : private flatbuffers::Table {
  typedef MessageBuilder Builder;
  enum FlatBuffersVTableOffset FLATBUFFERS_VTABLE_UNDERLYING_TYPE {
    VT_PAYLOAD_TYPE = 4,
    VT_PAYLOAD = 6
  };
  AmpedUpMessaging::MessagePayload payload_type() const {
    return static_cast<AmpedUpMessaging::MessagePayload>(GetField<uint8_t>(VT_PAYLOAD_TYPE, 0));
  }
  const void *payload() const {
    return GetPointer<const void *>(VT_PAYLOAD);
  }
  template<typename T> const T *payload_as() const;
  const AmpedUpMessaging::SetKnobValuesMessage *payload_as_SetKnobValuesMessage() const {
    return payload_type() == AmpedUpMessaging::MessagePayload::SetKnobValuesMessage ? static_cast<const AmpedUpMessaging::SetKnobValuesMessage *>(payload()) : nullptr;
  }
  const AmpedUpMessaging::SavePresetMessage *payload_as_SavePresetMessage() const {
    return payload_type() == AmpedUpMessaging::MessagePayload::SavePresetMessage ? static_cast<const AmpedUpMessaging::SavePresetMessage *>(payload()) : nullptr;
  }
  const AmpedUpMessaging::SaveCurrentAsPresetMessage *payload_as_SaveCurrentAsPresetMessage() const {
    return payload_type() == AmpedUpMessaging::MessagePayload::SaveCurrentAsPresetMessage ? static_cast<const AmpedUpMessaging::SaveCurrentAsPresetMessage *>(payload()) : nullptr;
  }
  const AmpedUpMessaging::LoadPresetMessage *payload_as_LoadPresetMessage() const {
    return payload_type() == AmpedUpMessaging::MessagePayload::LoadPresetMessage ? static_cast<const AmpedUpMessaging::LoadPresetMessage *>(payload()) : nullptr;
  }
  const AmpedUpMessaging::LogMessage *payload_as_LogMessage() const {
    return payload_type() == AmpedUpMessaging::MessagePayload::LogMessage ? static_cast<const AmpedUpMessaging::LogMessage *>(payload()) : nullptr;
  }
  bool Verify(flatbuffers::Verifier &verifier) const {
    return VerifyTableStart(verifier) &&
           VerifyField<uint8_t>(verifier, VT_PAYLOAD_TYPE) &&
           VerifyOffset(verifier, VT_PAYLOAD) &&
           VerifyMessagePayload(verifier, payload(), payload_type()) &&
           verifier.EndTable();
  }
};

template<> inline const AmpedUpMessaging::SetKnobValuesMessage *Message::payload_as<AmpedUpMessaging::SetKnobValuesMessage>() const {
  return payload_as_SetKnobValuesMessage();
}

template<> inline const AmpedUpMessaging::SavePresetMessage *Message::payload_as<AmpedUpMessaging::SavePresetMessage>() const {
  return payload_as_SavePresetMessage();
}

template<> inline const AmpedUpMessaging::SaveCurrentAsPresetMessage *Message::payload_as<AmpedUpMessaging::SaveCurrentAsPresetMessage>() const {
  return payload_as_SaveCurrentAsPresetMessage();
}

template<> inline const AmpedUpMessaging::LoadPresetMessage *Message::payload_as<AmpedUpMessaging::LoadPresetMessage>() const {
  return payload_as_LoadPresetMessage();
}

template<> inline const AmpedUpMessaging::LogMessage *Message::payload_as<AmpedUpMessaging::LogMessage>() const {
  return payload_as_LogMessage();
}

struct MessageBuilder {
  typedef Message Table;
  flatbuffers::FlatBufferBuilder &fbb_;
  flatbuffers::uoffset_t start_;
  void add_payload_type(AmpedUpMessaging::MessagePayload payload_type) {
    fbb_.AddElement<uint8_t>(Message::VT_PAYLOAD_TYPE, static_cast<uint8_t>(payload_type), 0);
  }
  void add_payload(flatbuffers::Offset<void> payload) {
    fbb_.AddOffset(Message::VT_PAYLOAD, payload);
  }
  explicit MessageBuilder(flatbuffers::FlatBufferBuilder &_fbb)
        : fbb_(_fbb) {
    start_ = fbb_.StartTable();
  }
  flatbuffers::Offset<Message> Finish() {
    const auto end = fbb_.EndTable(start_);
    auto o = flatbuffers::Offset<Message>(end);
    return o;
  }
};

inline flatbuffers::Offset<Message> CreateMessage(
    flatbuffers::FlatBufferBuilder &_fbb,
    AmpedUpMessaging::MessagePayload payload_type = AmpedUpMessaging::MessagePayload::NONE,
    flatbuffers::Offset<void> payload = 0) {
  MessageBuilder builder_(_fbb);
  builder_.add_payload(payload);
  builder_.add_payload_type(payload_type);
  return builder_.Finish();
}

inline bool VerifyMessagePayload(flatbuffers::Verifier &verifier, const void *obj, MessagePayload type) {
  switch (type) {
    case MessagePayload::NONE: {
      return true;
    }
    case MessagePayload::SetKnobValuesMessage: {
      auto ptr = reinterpret_cast<const AmpedUpMessaging::SetKnobValuesMessage *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessagePayload::SavePresetMessage: {
      auto ptr = reinterpret_cast<const AmpedUpMessaging::SavePresetMessage *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessagePayload::SaveCurrentAsPresetMessage: {
      auto ptr = reinterpret_cast<const AmpedUpMessaging::SaveCurrentAsPresetMessage *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessagePayload::LoadPresetMessage: {
      auto ptr = reinterpret_cast<const AmpedUpMessaging::LoadPresetMessage *>(obj);
      return verifier.VerifyTable(ptr);
    }
    case MessagePayload::LogMessage: {
      auto ptr = reinterpret_cast<const AmpedUpMessaging::LogMessage *>(obj);
      return verifier.VerifyTable(ptr);
    }
    default: return true;
  }
}

inline bool VerifyMessagePayloadVector(flatbuffers::Verifier &verifier, const flatbuffers::Vector<flatbuffers::Offset<void>> *values, const flatbuffers::Vector<uint8_t> *types) {
  if (!values || !types) return !values && !types;
  if (values->size() != types->size()) return false;
  for (flatbuffers::uoffset_t i = 0; i < values->size(); ++i) {
    if (!VerifyMessagePayload(
        verifier,  values->Get(i), types->GetEnum<MessagePayload>(i))) {
      return false;
    }
  }
  return true;
}

}  // namespace AmpedUpMessaging

#endif  // FLATBUFFERS_GENERATED_AMPEDUPMESSAGING_AMPEDUPMESSAGING_H_