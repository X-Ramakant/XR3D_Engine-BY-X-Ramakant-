# XR3D Engine — परियोजना दस्तावेज़ (Root README)

**विकासकर्ता:** XR Corporation
**संस्थापक:** रामकांत
**परियोजना का नाम:** XR3D Engine
**प्रकार:** मॉड्यूलर, क्रॉस-प्लेटफ़ॉर्म, हाई-परफॉर्मेंस 3D गेम इंजन

---

## इस दस्तावेज़ का उद्देश्य

यह दस्तावेज़ XR3D Engine परियोजना के **हर Git टैग** का पूरा, विस्तृत और
गहराई से लिखा गया इतिहास है। इसका मकसद यह है कि भविष्य में कोई भी
व्यक्ति — चाहे वह इस परियोजना का मूल डेवलपर हो या कोई नया व्यक्ति जो
पहली बार इस कोडबेस को देख रहा हो — बिना किसी और से कुछ पूछे यह समझ सके
कि:

- हर टैग पर वास्तव में क्या बनाया गया था
- वह क्यों बनाया गया था (उसकी ज़रूरत क्या थी)
- उसकी आंतरिक संरचना (architecture) कैसी है
- हर फ़ाइल की ज़िम्मेदारी क्या है
- कौन-कौन से डिज़ाइन निर्णय (design decisions) लिए गए और क्यों लिए गए
- कैसे उसे टेस्ट किया गया, और परीक्षण में क्या-क्या जाँचा गया
- कोई समस्या (bug) आई थी तो वह कैसे हल हुई
- भविष्य में उस मॉड्यूल में क्या-क्या और जोड़ा जाना बाकी है

यह दस्तावेज़ हर नए टैग के साथ आगे बढ़ता रहेगा — जैसे-जैसे नए मॉड्यूल
पूरे होंगे, वैसे-वैसे इसमें नए सेक्शन जुड़ते जाएँगे। अभी तक कुल **3 टैग**
पूरे हो चुके हैं, और इस दस्तावेज़ में उन तीनों का पूरा विवरण नीचे दिया
गया है।

---

## परियोजना का सामान्य परिचय

XR3D Engine एक ऐसा 3D गेम इंजन है जिसे शुरुआत से ही एक **व्यावसायिक,
मॉड्यूलर, स्केलेबल, मेंटेनेबल और रनटाइम-स्थिर** इंजन के रूप में विकसित
किया जा रहा है। इस परियोजना का मुख्य सिद्धांत यह है कि केवल फीचर बनाना
लक्ष्य नहीं है — बल्कि आर्किटेक्चर, सही-सही काम करना (correctness),
स्थिरता (stability), मेमोरी सुरक्षा, टेस्ट किए जाने की क्षमता
(testability), मेंटेन किए जाने की क्षमता, स्केलेबिलिटी, और नियंत्रित
विकास (controlled development) — यह सभी मिलकर असली लक्ष्य हैं।

परियोजना का विकास एक बहुत ही सख़्त नियम के तहत होता है: **एक समय में
केवल एक ही मॉड्यूल पर काम किया जाता है।** जिस मॉड्यूल पर काम शुरू होता
है, वह पूरी तरह पूर्ण (complete) हुए बिना अगला मॉड्यूल शुरू नहीं किया
जाता। हर मॉड्यूल को पूर्ण मानने के लिए एक निश्चित पाइपलाइन (pipeline)
से गुज़रना पड़ता है:

```
आवश्यकता परिभाषा (Requirement Definition)
        ↓
आर्किटेक्चर परिभाषा (Architecture Definition)
        ↓
निर्भरता विश्लेषण (Dependency Analysis)
        ↓
उप-घटक डिज़ाइन (Sub-component Design)
        ↓
फ़ाइल संरचना डिज़ाइन (File Structure Design)
        ↓
CMake डिज़ाइन
        ↓
क्रियान्वयन (Implementation)
        ↓
बिल्ड सत्यापन (Build Validation)
        ↓
यूनिट टेस्टिंग (Unit Testing)
        ↓
बग फिक्स (Bug Fix)
        ↓
पुनः-परीक्षण (Re-test)
        ↓
इंजन एकीकरण (Engine Integration)
        ↓
इंटीग्रेशन टेस्टिंग (Integration Testing)
        ↓
मेमोरी/रिसोर्स समीक्षा (Memory/Resource Review)
        ↓
रनटाइम स्थिरता समीक्षा (Runtime Stability Review)
        ↓
प्रलेखन (Documentation)
        ↓
मॉड्यूल पूर्णता समीक्षा (Module Completion Review)
        ↓
Git Commit
        ↓
Git Tag
        ↓
Git Push
        ↓
अगला मॉड्यूल
```

इस पूरे दस्तावेज़ में हर टैग को इसी पाइपलाइन के हिसाब से समझाया गया है।

---

## CMake Build Target Architecture (सामान्य जानकारी, हर टैग में लागू)

पूरी परियोजना में हमेशा केवल **3 मुख्य (primary) build targets** रहते
हैं:

1. **`XR3D_Engine`** — असली, चलने वाला (production/runtime) इंजन। यह
   `Engine\` फ़ोल्डर के अंदर मौजूद हर `.cpp`/`.h` फ़ाइल को, साथ ही
   जड़ (root) में मौजूद `Main.cpp` को कंपाइल करके एक चलने योग्य
   `.exe` बनाता है।

2. **`XR3D_UnitTests`** — मॉड्यूलों के अलग-थलग (isolated) यूनिट टेस्ट
   चलाने के लिए। इसे `Engine\` की सभी फ़ाइलें, साथ ही `Tests\Unit\`
   फ़ोल्डर की सभी फ़ाइलें मिलती हैं। इसे `Integration\` फ़ोल्डर का
   कोई एक्सेस नहीं मिलता।

3. **`XR3D_Integration`** — पूरा हो चुके मॉड्यूलों को इंजन के साथ
   मिलाकर टेस्ट करने के लिए। इसे `Engine\` की सभी फ़ाइलें, साथ ही
   `Integration\Scenarios\` फ़ोल्डर की सभी फ़ाइलें मिलती हैं। इसे
   `Tests\` फ़ोल्डर का कोई एक्सेस नहीं मिलता।

तीनों targets के बीच का यह अलगाव इसलिए ज़रूरी है ताकि यूनिट टेस्ट और
इंटीग्रेशन टेस्ट एक-दूसरे में हस्तक्षेप (interfere) न करें, और हर एक का
उद्देश्य साफ़-साफ़ अलग रहे — यूनिट टेस्ट सिर्फ़ एक छोटी इकाई (unit) को
अलग करके जाँचता है, जबकि इंटीग्रेशन टेस्ट यह जाँचता है कि असली इंजन
जैसी परिस्थितियों में वह इकाई सही व्यवहार करती है या नहीं।

**महत्वपूर्ण नियम:** दोनों टेस्टिंग targets के लिए **कोई भी थर्ड-पार्टी
लाइब्रेरी (जैसे GoogleTest) डिफ़ॉल्ट रूप से इस्तेमाल नहीं होती।** इसकी
जगह परियोजना का अपना, बहुत हल्का (lightweight), खुद लिखा हुआ टेस्ट
फ़्रेमवर्क इस्तेमाल होता है — `Tests\Unit\TestFramework.h` (यूनिट
टेस्ट के लिए, मैक्रो `XR3D_TEST`/`XR3D_CHECK`) और
`Integration\IntegrationTestFramework.h` (इंटीग्रेशन टेस्ट के लिए,
मैक्रो `XR3D_SCENARIO`/`XR3D_CHECK`) — दोनों जानबूझकर एक-दूसरे की
अलग-अलग प्रतियाँ (copies) हैं, ताकि `Integration` target कभी भी
`Tests\` फ़ोल्डर पर निर्भर न हो।

---

## प्रोजेक्ट-व्यापी लेखन नियम (सभी मॉड्यूलों में लागू)

हर टैग को समझने से पहले यह जान लेना ज़रूरी है कि पूरी परियोजना में कुछ
नियम हमेशा लागू होते हैं:

1. **हर `.h`/`.hpp` फ़ाइल के सबसे ऊपर** यह comment अनिवार्य है:
   ```
   Development By : XR Corporation
   Software Name : XR 3D Engine
   Founder Of XR Corporation : Ramakant
   File Responsibility: <उस फ़ाइल की असली ज़िम्मेदारी>
   ```

2. **हर `.cpp` फ़ाइल** में वही ऊपर वाला comment होता है, **और साथ ही
   सबसे नीचे** एक "Future Comment" वाला numbered comment भी होता है,
   जिसमें आगे किए जाने वाले सुधारों की सूची होती है।

3. **हेडर-ओनली नियम:** जो चीज़ें सिर्फ़ Types/Enums/सरल Structs/
   Constants/Templates हैं, उन्हें सिर्फ़ `.h` फ़ाइल में रखा जाता है
   — बिना वजह `.cpp` नहीं बनाई जाती। इससे फ़ाइलों की संख्या बेवजह नहीं
   बढ़ती।

4. **"Advanced/Future-Proof" लेखन शैली:** हर मॉड्यूल को शुरू से ही
   पूरी तरह उन्नत (advanced) और भविष्य के लिए तैयार (future-proof)
   तरीके से लिखा जाता है, ताकि बाद में बड़े बदलाव (breaking changes)
   न करने पड़ें। जहाँ अभी कोई चीज़ पूरी तरह लागू नहीं की जा सकती (जैसे
   किसी दूसरे मॉड्यूल पर निर्भरता जो अभी बना ही नहीं है), वहाँ साफ़
   `// TODO (Future): ...` comment छोड़ा जाता है।

5. **कोई भी module दूसरे module के internal implementation पर
   सीधे निर्भर नहीं होता।** हर module का एक "Manager" (single entry
   point) होता है, और बाकी सब उसी के through बाहर से access होता है।

---

<br>

# टैग 1 — `v0.1.0` (Project Setup / परियोजना की नींव)

## सारांश तालिका

| क्षेत्र | विवरण |
|---|---|
| टैग नाम | `v0.1.0` |
| संदेश | "Project Setup" |
| दिनांक-चरण | परियोजना की शुरुआत, कोई फीचर मॉड्यूल अभी नहीं बना |
| मुख्य उद्देश्य | संपूर्ण नींव (skeleton) तैयार करना — फ़ोल्डर संरचना, CMake, Git |
| फ़ाइलें जोड़ी गईं | 369 (202 `.h` + 167 `.cpp`, सभी खाली/stub फ़ाइलें, सही header comment के साथ) |
| Targets बने | `XR3D_Engine` (बिल्ड-सत्यापित), `XR3D_UnitTests` व `XR3D_Integration` (अभी खाली प्लेसहोल्डर) |

## यह टैग क्यों बनाया गया

किसी भी बड़े सॉफ़्टवेयर प्रोजेक्ट को बनाने से पहले उसकी **नींव**
(foundation) मज़बूत होनी चाहिए। अगर सीधे कोड लिखना शुरू कर दिया जाए
बिना यह सोचे कि फ़ोल्डर कैसे व्यवस्थित होंगे, बिल्ड सिस्टम कैसे काम
करेगा, और टेस्टिंग की संरचना क्या होगी — तो आगे चलकर बार-बार पूरे
प्रोजेक्ट को दोबारा व्यवस्थित (restructure) करना पड़ता है, जो समय की
बहुत बड़ी बर्बादी है।

इसीलिए इस टैग का उद्देश्य था: **कोई भी असली फीचर बनाने से पहले, पूरे
इंजन का "कंकाल" (skeleton) तैयार करना** — ताकि जब असली मॉड्यूल (जैसे
Memory, Logging) बनने शुरू हों, तो उन्हें बस सही जगह पर, सही तरीके से
जोड़ा जा सके, बिना संरचना के बारे में बार-बार सोचे।

## परियोजना की जड़ (Root) की संरचना

`D:\XR3D_Engine\` के अंदर यह टॉप-लेवल संरचना बनाई गई:

```
XR3D_Engine/
│
├── CMakeLists.txt          (जड़ स्तर — सिर्फ़ 3 subfolder को जोड़ता है)
├── CMakePresets.json        (Visual Studio 17 2022, x64, build/ में जनरेट)
├── README.md
├── DOCUMENTATION.md
├── .gitignore
├── Main.cpp                 (इंजन का entry point)
│
├── CMake/                   (भविष्य के लिए — साझा .cmake हेल्पर स्क्रिप्ट)
├── Config/                  (भविष्य के लिए — वैश्विक कॉन्फ़िगरेशन)
│
├── Engine/                  (असली इंजन कोड — नीचे विस्तार से)
│
├── Tests/
│   ├── CMakeLists.txt        (अभी खाली था इस टैग में)
│   └── Unit/
│       ├── Core/
│       └── Engine/
│
├── Integration/
│   ├── CMakeLists.txt        (अभी खाली था इस टैग में)
│   └── Scenarios/
│       ├── Core/
│       └── Engine/
│
├── Docs/
│   └── DevelopmentLog/
│       ├── Core/
│       └── Modules/
│
├── Editor/, Runtime/, Launcher/, Tools/, Benchmarks/, Examples/,
│   Assets/, Projects/, ThirdParty/, Generated/, Documentation/,
│   Scripts/, Build/     (यह सभी अभी सिर्फ़ खाली फ़ोल्डर हैं — भविष्य
│                          के चरणों के लिए आरक्षित; अभी इनके अंदर कोई
│                          फ़ाइल नहीं बनाई गई, क्योंकि "बिना ज़रूरत के
│                          कोई फ़ाइल/फ़ोल्डर मत बनाओ" वाला नियम है)
```

यह टॉप-लेवल संरचना Unreal Engine, Godot जैसे असली production इंजनों की
तरह ही अलग-अलग बड़े हिस्सों को अलग रखती है — जैसे Engine (कोर इंजन),
Editor (भविष्य में बनने वाला Qt-आधारित संपादक), Runtime (स्टैंडअलोन
रनटाइम), Launcher (प्रोजेक्ट लॉन्चर)।

## `Engine\` फ़ोल्डर की विस्तृत संरचना

यह इस टैग का सबसे बड़ा हिस्सा है। `Engine\` के अंदर दो मुख्य भाग हैं:

### भाग 1 — `Engine\Platform\` (OS Abstraction परत)

```
Engine/Platform/
├── Core/                     (OS-निरपेक्ष सार, केवल अमूर्त इंटरफ़ेस)
│   ├── PlatformInterface.h
│   ├── PlatformManager.h/.cpp
│   ├── PlatformCapabilities.h/.cpp
│   └── PlatformTypes.h
│
├── Windows/                  (सक्रिय विकास मंच — 13 उप-प्रणालियाँ)
│   ├── Window/, Dialog/, Display/, Environment/, Event/,
│   │   FileSystem/, Input/, Library/, Monitor/, Process/,
│   │   Registry/, Security/, Shell/
│   (हर एक के अंदर WindowsXXX.h/.cpp)
│
├── Linux/                    (भविष्य — 12 उप-प्रणालियाँ, Registry नहीं)
└── macOS/                    (भविष्य — 12 उप-प्रणालियाँ, Registry नहीं)
```

**यह क्यों ज़रूरी है:** `Platform` परत का काम है — "OS से बात कैसे
करनी है" यह तय करना। जैसे विंडो बनाना, फ़ाइलें पढ़ना, प्रोसेस चलाना —
यह सब OS-विशिष्ट (OS-specific) काम हैं। अगर `Core` के अन्दर के सिस्टम
(जैसे Memory, Logging) सीधे Windows API कॉल करने लगें, तो भविष्य में
Linux/macOS पर पोर्ट करना बहुत मुश्किल हो जाएगा। इसलिए एक अलग `Platform`
परत बनाई गई जो हर OS का ब्यौरा छुपाकर एक समान (uniform) इंटरफ़ेस देती
है।

`Platform\Windows\` को अभी पूरी तरह विस्तार से (13 उप-प्रणालियों के
साथ) बनाया गया है क्योंकि डेवलपमेंट अभी Windows पर हो रहा है। `Linux`
और `macOS` को सिर्फ़ फ़ोल्डर-स्केलेटन के रूप में छोड़ा गया है — जब
असल में उनका implementation करना होगा, तभी उनकी फ़ाइलें भरी जाएँगी।

### भाग 2 — `Engine\Core\` (15 आधारभूत प्रणालियाँ)

`Core` वह परत है जिसे **पूरा इंजन सीधे, हमेशा, बिना किसी विकल्प के
इस्तेमाल करता है।** चाहे Window मॉड्यूल हो, Rendering हो, Physics हो —
सब कुछ नीचे जाकर Core तक पहुँचता है। इस टैग में Core के अंदर 15
प्रणालियाँ बनाई गईं (सिर्फ़ ढाँचा — खाली फ़ाइलें):

```
Engine/Core/
├── CoreManager.h/.cpp        (सभी 15 प्रणालियों का संचालक)
├── Memory/                   (Manager, Allocator, Pool, Tracker, Types)
├── Logging/                  (Logger, LogSink, LogFormatter, LogRegistry, LogTypes)
├── Config/                   (Manager, Registry, Loader, Types)
├── Time/                     (Manager, Registry, Clock, Timer, Types, Handle)
├── FileSystem/                (Manager, Path, Watcher, Registry, Types)
├── Threading/                 (ThreadManager, ThreadPool, Types)
├── JobSystem/                 (Manager, Queue, Scheduler, Types)
├── Events/                    (Manager, Queue, Dispatcher, Registry, Types)
├── Reflection/                 (Manager, Registry, Descriptor, Types)
├── Serialization/               (Manager, Registry, StreamBinary, StreamJson, Types)
├── ObjectSystem/                (Manager, Registry, Lifetime, Types, Handle)
├── Services/                    (Manager, Table, Types)
├── Resource/                    (Manager, Registry, Loader, Types, Handle)
├── PluginSystem/                 (Manager, Registry, Loader, Types)
└── Diagnostics/                  (Manager, Assert, CrashHandler, Types)
```

हर एक प्रणाली के लिए एक ही जैसा (consistent) पैटर्न अपनाया गया —
`<System>Manager` (मुख्य संचालक, बाहर से एकमात्र संपर्क बिंदु),
`<System>Types.h` (हमेशा header-only, Types/Enums के लिए), और फिर
ज़रूरत के हिसाब से `Registry`, `Loader`, `Handle` जैसे अतिरिक्त हिस्से
— लेकिन **हर सिस्टम को सब कुछ ज़बरदस्ती नहीं दिया गया**, सिर्फ़ जो
वाकई चाहिए वही (उदाहरण: `Logging` को `Handle` फ़ाइल नहीं चाहिए,
`Threading` को `Registry` नहीं चाहिए)।

### भाग 3 — `Engine\Window\`, `Input\`, `UI\`, `Rendering\` (बड़े feature मॉड्यूल)

इन चार मॉड्यूलों को — क्योंकि यह भविष्य में बहुत बड़े होने वाले हैं —
अभी से ही उप-प्रणालियों (subsystems) में बाँट दिया गया, ताकि बाद में
पूरे मॉड्यूल को दोबारा-व्यवस्थित न करना पड़े:

```
Window/    → Core, Events, Properties, Display, Cursor, Platform(bridge)
Input/     → Core, Keyboard, Mouse, Gamepad, Touch, XR, Mapping
UI/        → Core, Widgets, Layout, Events, Input, Theme, Resources, Qt(bridge)
Rendering/ → Core, Vulkan(Instance/Device/Surface/Swapchain/Command/
             Synchronization/Memory/Resources/Descriptors), Pipeline,
             Pass, Frame, Resources, Shaders, Debug
```

**"Bridge" फ़ाइलों का मतलब:** `Window\Platform\WindowPlatformBridge.h`
और `UI\Qt\UIQtBridge.h` जैसी फ़ाइलें सिर्फ़ एक पतला (thin) सम्पर्क-सूत्र
हैं। `Window` मॉड्यूल खुद कभी सीधे Win32 कोड नहीं लिखता — वह
`WindowPlatformBridge` के through `Engine\Platform\Windows\Window\`
तक पहुँचता है। इसी तरह Engine का असली Core कभी Qt पर निर्भर नहीं होता —
Qt सिर्फ़ भविष्य के Editor में इस्तेमाल होगा, `UI\Qt\` सिर्फ़ एक पतला
पुल है।

### भाग 4 — बाकी बचे 11 feature मॉड्यूल (V5-V15, अभी सपाट/flat)

```
Scripting/, Assets/, Physics/, Audio/, Plugins/, Networking/,
Simulation/, Media/, Utilities/, Extras/, AI/
```

इन 11 मॉड्यूलों को अभी उप-प्रणालियों में नहीं बाँटा गया, क्योंकि इनकी
बारी अभी नहीं आई (यह V5 से V15 के विकास-चरण में बनेंगे)। हर एक के अंदर
सिर्फ़ बुनियादी फ़ाइलें (Manager + कुछ specific हिस्से + Types) रखी
गई हैं।

## फ़ाइलों की कुल गिनती (इस टैग में)

| क्षेत्र | फ़ोल्डर | फ़ाइलें |
|---|---|---|
| Engine/Platform (Core+Windows पूरा, Linux/macOS सिर्फ़ folder) | 41 | 41 |
| Engine/Core (15 systems) | 16 | 68 |
| Engine/Window | 6 | 8 |
| Engine/Input | 7 | 8 |
| Engine/UI | 8 | 9 |
| Engine/Rendering (+Vulkan) | 15 | 17 |
| Engine/(Scripting..AI, 11 मॉड्यूल) | 11 | 51 |
| जड़ स्तर की फ़ाइलें (CMakeLists, README, आदि) | — | 6 |
| **कुल** | **~102 फ़ोल्डर** | **369 फ़ाइलें** |

## CMake — कैसे तीन Targets को जोड़ा-अलग रखा गया

जड़ स्तर की `CMakeLists.txt` सिर्फ़ इतना करती है:

```cmake
add_subdirectory(Engine)
add_subdirectory(Tests)
add_subdirectory(Integration)
```

`Engine\CMakeLists.txt` अकेले `XR3D_Engine` target बनाती है, और साथ
में तीन variables (`XR3D_ENGINE_SOURCES`, `XR3D_ENGINE_HEADERS`,
`XR3D_ENGINE_INCLUDE_DIR`) को `PARENT_SCOPE` के through ऊपर भेज देती
है, ताकि `Tests\` और `Integration\` अपने-अपने CMakeLists.txt में इन्हें
फिर से इस्तेमाल कर सकें — बिना Engine के कोड की एक और नकल (copy) बनाए।

`Tests\CMakeLists.txt` और `Integration\CMakeLists.txt` इस टैग में अभी
**बिल्कुल खाली** थीं (सिर्फ़ 0-byte प्लेसहोल्डर फ़ाइलें) — यह जानबूझकर
किया गया, क्योंकि जब तक उनके अंदर कोई असली टेस्ट फ़ाइल न हो, तब तक
उनका कोई target बनाना बेमानी है (नियम: बिना ज़रूरत के structure मत
बनाओ)।

## Build Validation — क्या हुआ

इस टैग में **`XR3D_Engine`** target को Visual Studio 17 2022 (x64) से
सफलतापूर्वक बनाया गया — सभी 167 `.cpp` फ़ाइलें (जो सभी अभी सिर्फ़
comment वाली खाली फ़ाइलें थीं, कोई असली कोड नहीं) बिना किसी compile
error के compile हुईं, और `D:\XR3D_Engine\build\Engine\Debug\
XR3D_Engine.exe` सफलतापूर्वक बनी।

## शुरुआत में आई समस्याएँ और उनका समाधान

इस टैग को स्थापित करते वक़्त कई CMake से जुड़ी समस्याएँ आईं, जो सामान्य
CMake-configuration समस्याएँ थीं:

1. **"source directory does not appear to contain CMakeLists.txt"** —
   जड़ स्तर की `CMakeLists.txt` कभी भरी ही नहीं गई थी (खाली रह गई थी)।
   हल: root CMakeLists.txt में असली content लिखा गया (project()
   declaration + तीन add_subdirectory calls)।

2. **"add_subdirectory given source Tests which is not an existing
   directory"** — `Tests\` और `Integration\` फ़ोल्डर पहले कभी बने ही
   नहीं थे (शुरुआती स्क्रिप्ट चली नहीं थी)। हल: दोनों फ़ोल्डर, उनके
   उप-फ़ोल्डर, और खाली `CMakeLists.txt` placeholder बनाए गए।

3. **"Cannot find source file: Main.cpp" / "No SOURCES given to
   target"** — जड़ स्तर की `Main.cpp` भी कभी बनी नहीं थी। हल: एक
   न्यूनतम, काम करने वाला `main()` स्टब बनाया गया।

4. **Stale Visual Studio cache** — `.vs\` फ़ोल्डर की फ़ाइलें "process
   cannot access" त्रुटि दे रही थीं क्योंकि Visual Studio अभी भी खुला
   था। हल: Visual Studio को पूरी तरह बंद करके, `.vs\` और `build\`
   फ़ोल्डर हटाकर, ताज़ा (fresh) खोला गया।

## Git History इस टैग तक

```
git init
git add .
git commit -m "chore(project): initialize XR 3D Engine foundation"
git tag -a v0.1.0 -m "Project Setup"
git push -u origin main
git push origin v0.1.0
```

बाद में एक ब्रांच-भ्रम (branch confusion) की समस्या भी हल की गई —
GitHub पर डिफ़ॉल्ट ब्रांच `master` सेट थी जबकि असली काम `main` ब्रांच
में था। GitHub CLI (`gh`) के through डिफ़ॉल्ट ब्रांच `main` सेट की गई,
और पुरानी `master` ब्रांच (जिसमें सिर्फ़ GitHub के अपने-आप बनाए हुए 2
शुरुआती commit थे) हटाई गई।

`.gitignore` को भी इस टैग के दौरान पूरी तरह नया बनाया गया — CMake की
`build\`/`Build\`, Visual Studio की `.vs\`/`*.vcxproj*`/`*.sln`,
Debug/Release output, compiled binaries (`*.obj`/`*.exe`/`*.pdb`), और
भविष्य के लिए Qt/Vulkan के generated files — सब कुछ इसमें शामिल किया
गया।

## इस टैग से मिली सीख

- किसी भी बड़े प्रोजेक्ट को शुरू करने से पहले, folder structure और
  build system को पूरी तरह सोच-समझकर तय करना चाहिए — बाद में बदलना
  महंगा (costly) होता है।
- CMake में `CONFIGURE_DEPENDS` के साथ `file(GLOB_RECURSE ...)` इस्तेमाल
  करना, बहुत सारी फ़ाइलों वाले प्रोजेक्ट के लिए व्यावहारिक (pragmatic)
  है — इसे हर बार मैन्युअल रूप से फ़ाइल-लिस्ट अपडेट करने से बेहतर माना
  गया, भले ही यह "शुद्ध" (purist) CMake तरीका न हो।
- `.gitignore` को शुरू में ही सही तरीके से बनाना ज़रूरी है, वरना बाद
  में generated files repository में चले जाते हैं और उन्हें
  `git rm -r --cached` से साफ़ करना पड़ता है।

## यह टैग समग्र योजना में कहाँ फिट होता है

यह टैग परियोजना के Rule 34 (Initial Project Commit) को पूरा करता है —
"वर्तमान स्थिति में केवल project foundation/base architecture तैयार
है, initial commit का उद्देश्य यह record करना है कि project का initial
foundation स्थापित हो चुका है।" इसके बाद से हर अगला टैग एक-एक करके Core
की 15 प्रणालियों को असली, काम करने वाले कोड से भरता जाएगा — जिसकी
शुरुआत अगले टैग (`v0.2.0-memory`) से होती है।

---

<br>

# टैग 2 — `v0.2.0-memory` (Core / Memory मॉड्यूल)

## सारांश तालिका

| क्षेत्र | विवरण |
|---|---|
| टैग नाम | `v0.2.0-memory` |
| संदेश | "Core Memory module complete: 9 unit tests, 4 integration tests, wired into Engine" |
| मॉड्यूल समूह | Core |
| संस्करण चरण | V1 |
| Core में क्रम | 15 में से पहली प्रणाली |
| यूनिट टेस्ट | 9/9 पास |
| इंटीग्रेशन टेस्ट | 4/4 पास |

## यह मॉड्यूल क्यों सबसे पहले बनाया गया

Memory Core की 15 प्रणालियों में से **सबसे नीचे की नींव** है। Logging,
Config, Time, FileSystem — यह सभी प्रणालियाँ अपने अंदर वस्तुएँ (objects)
और बफ़र (buffers) बनाएँगी, और वह सब कुछ शुरू से ही एक ही, ट्रैक किए जाने
योग्य (trackable), leak-सुरक्षित रास्ते से गुज़रना चाहिए। इसलिए Memory
को सबसे पहले, और पूरी तरह पूर्ण बनाना ज़रूरी था — ताकि बाद में बने
सिस्टमों में मेमोरी-ट्रैकिंग को दोबारा से जोड़ना (retrofit) न पड़े।

एक और वजह यह भी थी कि Memory को अभी **Platform की ज़रूरत नहीं है** —
यह सिर्फ़ मानक C++ (`malloc`/`free`) का इस्तेमाल करके काम कर सकता है।
इसलिए यह Platform बनने से पहले भी शुरू किया जा सकता था।

## डिज़ाइन निर्णय — "Option 4" (सब कुछ शुरू से ही उन्नत/advanced)

Memory मॉड्यूल को डिज़ाइन करते वक़्त एक महत्वपूर्ण फ़ैसला लिया गया —
इसे शुरुआत में सिर्फ़ बुनियादी allocate/free ट्रैकिंग तक सीमित नहीं
रखा गया, बल्कि **एक साथ सभी 5 allocator strategies** को शुरू से ही
पूरी तरह लागू किया गया, ताकि भविष्य में इसे दोबारा से (breaking change
के साथ) न बदलना पड़े। जहाँ कोई गहरी optimization अभी संभव नहीं थी
(जैसे free-list में block-coalescing), वहाँ साफ़ `TODO (Future)`
comment छोड़ा गया — लेकिन बुनियादी functionality पूरी तरह काम करने
योग्य (fully functional) है।

## आर्किटेक्चर — पूरा चित्रण

```
                     MemoryManager (singleton, single entry point)
                              |
        -----------------------------------------------------
        |            |             |              |          |
  DefaultAllocator LinearAllocator StackAllocator FreeListAllocator MemoryPool
   (malloc/free)     (bump, per-   (LIFO, marker/  (any-order free,   (fixed block
                      frame arena)  rewind)         whole-block reuse)  size, कई
                                                                        instance)
                              |
                       MemoryTracker
              (per-pointer records, global + per-tag stats,
                        leak detection, reporting)
```

सभी पाँचों allocator strategies एक ही common interface, `IAllocator`,
लागू (implement) करते हैं:

```cpp
class IAllocator
{
public:
    virtual ~IAllocator() = default;
    virtual void* Allocate(size_t size, size_t alignment = kDefaultAlignment) = 0;
    virtual void  Deallocate(void* pointer) = 0;
    virtual void  Reset() = 0;
    virtual const char* GetName() const = 0;
    virtual AllocatorType GetType() const = 0;
};
```

इससे `MemoryManager` को यह जानने की ज़रूरत नहीं पड़ती कि अंदर कौन-सा
allocator चल रहा है — वह सिर्फ़ एक जैसा (polymorphic) कॉल करता है।

## हर फ़ाइल की विस्तृत जानकारी

### `MemoryTypes.h` (header-only)

इस फ़ाइल में कोई भी implementation नहीं है, सिर्फ़ Types/Enums और एक
अपरिवर्तनीय (immutable) constant:

- `enum class AllocatorType` — Default, Linear, Stack, Pool, FreeList
- `enum class MemoryTag` — Untagged, Core, Rendering, Physics, Audio,
  Scripting, Networking, UI, AI, Resource — यह हर allocation को किस
  उप-प्रणाली से जुड़ा है, इसका लेबल देता है
- `constexpr size_t kDefaultAlignment` — जब कॉल करने वाला कोई alignment
  नहीं बताता, तो इस्तेमाल होने वाला डिफ़ॉल्ट alignment
- `struct AllocationRecord` — हर allocation के लिए दर्ज (record) की
  जाने वाली जानकारी: pointer, size, alignment, allocator, tag, source
  file, source line
- `struct MemoryStats` — currentUsage, peakUsage, totalAllocated,
  totalFreed, allocationCount, deallocationCount

### `MemoryAllocator.h` / `.cpp`

इस फ़ाइल-जोड़ी में `IAllocator` इंटरफ़ेस के साथ-साथ चार ठोस
(concrete) allocator classes हैं:

**`DefaultAllocator`** — बस सीधे OS heap का इस्तेमाल करता है
(`std::malloc`/`std::free`)। जब कोई खास ज़रूरत नहीं होती, तब यही
इस्तेमाल होता है।

**`LinearAllocator`** — एक तयशुदा आकार (fixed-size) के बफ़र में लगातार
आगे बढ़ता (bump) जाता है। व्यक्तिगत रूप से (individually) कुछ भी मुक्त
(free) नहीं किया जा सकता — पूरे बफ़र को एक साथ `Reset()` से खाली किया
जाता है। यह प्रति-फ़्रेम (per-frame) अस्थायी (scratch) मेमोरी के लिए
आदर्श है — जैसे किसी गेम-लूप में हर फ़्रेम का अस्थायी डेटा।

**`StackAllocator`** — LIFO (Last-In-First-Out) तरीके से काम करता है।
`GetMarker()` से एक "निशान" (marker) लिया जा सकता है, और
`RewindToMarker()` से उस निशान तक वापस जाया जा सकता है — जो कुछ भी उस
निशान के बाद allocate हुआ था, वह सब मुक्त हो जाता है। यह नेस्टेड
(nested) फ़ंक्शन कॉल्स में अस्थायी मेमोरी के लिए उपयोगी है।

**`FreeListAllocator`** — किसी भी क्रम (order) में allocate/free किया
जा सकता है। यह मुक्त किए गए ब्लॉकों की एक लिंक्ड-लिस्ट (`FreeBlock`)
बनाए रखता है। जब कोई नया allocation माँगा जाता है, यह सूची में पहला
ऐसा block ढूँढता है जो काफ़ी बड़ा हो (first-fit search)।

एक महत्वपूर्ण **सुरक्षा-सुधार (bug-fix)** इस मॉड्यूल में हुआ: शुरू में
तीनों (Linear, Stack, FreeList) allocators के constructor में, अगर
`std::malloc` किसी वजह से असफल (fail) हो जाता (जैसे बहुत बड़ी मेमोरी
माँगने पर), तो `m_buffer` `nullptr` बन जाता, और तुरंत बाद वाली लाइन
उसी `nullptr` को dereference कर देती — जिससे क्रैश हो सकता था। एक
स्टैटिक विश्लेषक (static analyzer) ने यह चेतावनी दी:
**"Dereferencing NULL pointer 'm_freeList'"**। इसे ठीक करने के लिए हर
constructor में `assert(m_buffer != nullptr && "...")` जोड़ा गया, और
`FreeListAllocator::Reset()` में भी `m_buffer == nullptr` की जाँच जोड़ी
गई इससे पहले कि वह उसे dereference करे। यही सुधार `MemoryPool.cpp` में
भी किया गया।

### `MemoryPool.h` / `.cpp`

यह पाँचवीं allocator strategy है, जो एक तयशुदा (fixed) block-size के
साथ काम करती है — जैसे अगर सभी blocks `sizeof(int)` के बराबर हों। यह
बहुत तेज़ (fast) allocate/free देता है क्योंकि इसमें कोई search नहीं
करनी पड़ती — बस एक free-list से अगला उपलब्ध node उठा लिया जाता है।
यह भविष्य में ECS Components या छोटी-छोटी बार-बार बनने वाली वस्तुओं के
लिए इस्तेमाल होगा। `MemoryManager::CreatePool()` से जितने चाहें उतने
अलग-अलग pool instance बनाए जा सकते हैं (हर एक की अपनी block-size के
साथ)।

### `MemoryTracker.h` / `.cpp`

यह क्लास हर सक्रिय (active) allocation का रिकॉर्ड रखती है, एक
`std::unordered_map<void*, AllocationRecord>` के रूप में। इसकी मुख्य
ज़िम्मेदारियाँ:

- `OnAllocate()` / `OnDeallocate()` — हर allocation/deallocation पर
  सांख्यिकी (statistics) को अपडेट करना, दोनों वैश्विक (global) स्तर
  पर और प्रत्येक Tag के हिसाब से अलग-अलग
- `TryGetRecord()` — किसी दिए गए pointer के लिए उसका रिकॉर्ड ढूँढना
  (यह `MemoryManager::Deallocate()` के लिए बहुत ज़रूरी है — नीचे देखें)
- `HasLeaks()` / `DumpReport()` — यह जाँचना कि कोई मेमोरी अभी भी मुक्त
  नहीं हुई है, और उसकी पूरी रिपोर्ट (कितनी बाइट्स, किस फ़ाइल/लाइन से)
  प्रिंट करना

पूरी क्लास एक `std::mutex` से सुरक्षित (protected) है, ताकि यह भविष्य
में multi-threaded इस्तेमाल के लिए भी सुरक्षित रहे।

### `MemoryManager.h` / `.cpp`

यह पूरे मॉड्यूल का **एकमात्र प्रवेश-बिंदु (single entry point)** है —
एक singleton class। इसकी सबसे महत्वपूर्ण डिज़ाइन-विशेषता यह है:

**समस्या:** अगर कोई वस्तु `LinearAllocator` से allocate की गई हो, और
कोई उसे साधारण `MemoryManager::Deallocate(ptr)` से मुक्त करने की
कोशिश करे — तो अगर `Deallocate()` हमेशा `DefaultAllocator` मान लेता
(जो `std::free()` कॉल करता), तो यह undefined behaviour (और संभावित
क्रैश) होता, क्योंकि वह pointer कभी `malloc()` से नहीं आया था।

**समाधान:** `MemoryManager::Deallocate()` पहले `MemoryTracker` से
`TryGetRecord()` के through यह पता लगाता है कि यह pointer असल में किस
allocator से आया था (यह जानकारी `AllocationRecord::allocator` में
पहले से दर्ज होती है, जब `Allocate()` कॉल हुआ था)। फिर वह उसी सही
allocator का `Deallocate()` कॉल करता है। इस तरह Engine का कोई भी हिस्सा
कभी भी गलत allocator को call नहीं कर सकता — यह गलती architecture के
स्तर पर ही असंभव बना दी गई।

`MemoryManager` तीन Linear/Stack/FreeList arenas को तयशुदा (fixed)
आकार के साथ बनाता है (`kLinearArenaSize = 8MB`, `kStackArenaSize =
4MB`, `kFreeListArenaSize = 16MB`) — इन्हें भविष्य में `Core/Config`
प्रणाली बनने के बाद configurable बनाया जाएगा (अभी TODO के रूप में
दर्ज)।

इसके साथ दो सुविधाजनक (convenience) मैक्रो भी हैं:

```cpp
#define XR3D_ALLOC(size, tag) \
    ::XR3D::Core::Memory::MemoryManager::Get().Allocate((size), \
        ::XR3D::Core::Memory::AllocatorType::Default, (tag), __FILE__, __LINE__)

#define XR3D_FREE(pointer) \
    ::XR3D::Core::Memory::MemoryManager::Get().Deallocate((pointer))
```

यह कॉल करने वाले की जगह से `__FILE__`/`__LINE__` को अपने आप पकड़ लेते
हैं, ताकि लीक होने पर यह पता चल सके कि वह allocation कहाँ से आई थी।

## उपयोग का उदाहरण

```cpp
#include "Core/Memory/MemoryManager.h"

XR3D::Core::Memory::MemoryManager::Get().Initialize();

void* data = XR3D_ALLOC(256, XR3D::Core::Memory::MemoryTag::Rendering);

XR3D_FREE(data);

XR3D::Core::Memory::MemoryManager::Get().Shutdown();
```

## यूनिट टेस्टिंग — 9/9 पास

`Tests\Unit\Core\MemoryTests.cpp` में यह 9 टेस्ट हैं:

1. **`Memory_Manager_InitializeAndShutdown`** — यह जाँचता है कि
   `Initialize()` को दो बार बुलाना सुरक्षित (safe no-op) है
2. **`Memory_DefaultAllocator_AllocateAndFree`** — allocate करने के
   बाद सांख्यिकी सही अपडेट होती है, और free करने के बाद वापस शून्य
   हो जाती है
3. **`Memory_PerTagStats_TrackSeparately`** — Rendering और Physics
   Tag की मेमोरी उपयोग अलग-अलग सही ढंग से track होती है
4. **`Memory_LinearAllocator_BumpAndReset`** — दो allocation अलग-अलग
   पते (addresses) देते हैं, और `Reset()` के बाद बफ़र वापस शुरुआती
   बिंदु से शुरू होता है
5. **`Memory_StackAllocator_MarkerRewind`** — marker लेने और
   rewind करने के बाद वही पता (address) दोबारा मिलता है
6. **`Memory_Pool_AllocateFreeAndBlockCount`** — pool से allocate/free
   करने पर `GetFreeBlockCount()` सही संख्या दिखाता है
7. **`Memory_FreeListAllocator_AllocateAndFree`** — allocate करने के
   बाद free करने पर, वही पूरा block दोबारा इस्तेमाल हो पाता है
8. **`Memory_Deallocate_RoutesToCorrectAllocator`** — Linear allocator
   से allocate करके generic `Deallocate()` से free करने पर कोई क्रैश
   नहीं होता (यह ठीक वही बग है जिसे ऊपर बताया गया)
9. **`Memory_NoLeaksAfterShutdown`** — Shutdown के बाद कोई मेमोरी
   बाकी नहीं बचती

## इंटीग्रेशन टेस्टिंग — 4/4 पास

`Integration\Scenarios\Core\MemoryIntegrationTests.cpp` में असली इंजन
जैसी परिस्थितियाँ बनाई गईं:

1. **`Integration_Memory_SimulatedFrameLoop_NoLeaks`** — 60 नकली
   (simulated) फ़्रेम, हर एक में Linear allocator से scratch मेमोरी
   लेकर `Reset()` करना — यह असली गेम-लूप जैसा पैटर्न है
2. **`Integration_Memory_MultiSystem_TaggedAllocations_NoLeaks`** —
   Rendering, Physics, Audio तीनों "उप-प्रणालियाँ" (simulate की गई)
   एक साथ allocate/free करती हैं, अंत में कोई लीक नहीं बचती
3. **`Integration_Memory_StackAllocator_NestedScopes`** — एक बाहरी
   और एक भीतरी marker/rewind — जैसे एक फ़ंक्शन के अंदर दूसरा फ़ंक्शन
   अपनी scratch मेमोरी इस्तेमाल करे
4. **`Integration_Memory_Pool_ExhaustionHandledGracefully`** — pool
   खत्म (exhausted) हो जाने पर वह क्रैश नहीं करता, बस gracefully
   `nullptr` लौटाता है

## Memory/Resource समीक्षा और Runtime Stability समीक्षा

इस चरण में कोई नया कोड नहीं लिखा गया, बस पुष्टि (confirm) की गई:

- Ownership स्पष्ट है — `MemoryManager` सभी allocators को
  `std::unique_ptr` से "own" करता है
- Shutdown का क्रम सही है — pools → freelist → stack → linear →
  default (यह Initialize के बिल्कुल उल्टे क्रम में है)
- Thread-safety — `MemoryTracker` mutex-सुरक्षित है
- कोई dangling pointer परिदृश्य (scenario) नहीं है (टेस्ट सूट से
  सत्यापित)

## Engine Integration

`Main.cpp` में `MemoryManager::Get().Initialize()` शुरुआत में और
`MemoryManager::Get().Shutdown()` अंत में जोड़ा गया।

## भविष्य के लिए बचे हुए काम (TODO)

- Linear/Stack/FreeList arenas के आकार को `Core/Config` के through
  configurable बनाना
- `FreeListAllocator` में best-fit search और आसन्न (adjacent) मुक्त
  ब्लॉकों का coalescing जोड़ना
- allocation-failure की रिपोर्टिंग को `Core/Diagnostics` के through
  भेजना (अभी सिर्फ़ `assert` है)
- Rendering (V4) के लिए NUMA-node/GPU-visible मेमोरी टैगिंग
- `MemoryPool` को बढ़ाने योग्य (growable) बनाना

## Git

```
git commit -m "feat(core): implement Memory subsystem (Default/Linear/Stack/Pool/FreeList allocators)"
git tag -a v0.2.0-memory -m "Core Memory module complete: 9 unit tests, 4 integration tests, wired into Engine"
```

## इस टैग से मिली सीख

- किसी भी allocator-आधारित सिस्टम में `malloc` की असफलता (failure) को
  हमेशा संभालना चाहिए, चाहे वह असल में कभी न हो — static analyzer ने
  यह गलती production कोड में जाने से पहले ही पकड़ ली।
- एक ही "generic" `Deallocate()` फ़ंक्शन बनाते वक़्त यह सोचना ज़रूरी
  है कि वह सही तरीके से यह तय कर पाए कि असल allocation कहाँ से आई थी
  — वरना यह एक गुप्त (silent), पकड़ में न आने वाला बग बन सकता है।

---

<br>

# टैग 3 — `v0.3.0-logging` (Core / Logging मॉड्यूल)

## सारांश तालिका

| क्षेत्र | विवरण |
|---|---|
| टैग नाम | `v0.3.0-logging` |
| संदेश | "Core Logging module complete: 6 unit tests, 3 integration tests, wired into Engine" |
| मॉड्यूल समूह | Core |
| संस्करण चरण | V1 |
| Core में क्रम | 15 में से दूसरी प्रणाली |
| यूनिट टेस्ट | 6/6 पास |
| इंटीग्रेशन टेस्ट | 3/3 पास |

## यह मॉड्यूल क्यों दूसरे नंबर पर बनाया गया

Memory के तुरंत बाद Logging बनाने का कारण यह था कि इससे आगे बनने वाली
हर प्रणाली (Config, Time, FileSystem, और आगे) को अपनी स्थिति, त्रुटियाँ
(errors), और चेतावनियाँ (warnings) रिपोर्ट करने के लिए एक भरोसेमंद
तरीका चाहिए। अगर Logging बाद में बनता, तो शुरुआती प्रणालियों को
अस्थायी रूप से (temporarily) सीधे `printf` इस्तेमाल करना पड़ता, जिसे
बाद में बदलना पड़ता — यह ठीक वही समस्या है जिससे "Advanced/Future-Proof"
नियम बचाना चाहता है।

## आर्किटेक्चर — पूरा चित्रण

```
                    Logger  (singleton — एकमात्र बाहरी संपर्क बिंदु)
                       |
                 LogRegistry
        (level filter + category filter, sinks की सूची रखता है)
                       |
        -----------------------------------
        |                                 |
   ConsoleSink                        FileSink
  (stdout/stderr)                  (टेक्स्ट फ़ाइल में लिखता है)
                       |
                 ILogFormatter
       (LogMessage को अंतिम प्रिंट-योग्य string में बदलता है)
```

## हर फ़ाइल की विस्तृत जानकारी

### `LogTypes.h` (header-only)

- `enum class LogLevel` — Trace, Debug, Info, Warning, Error, Fatal
  (छह स्तर, गंभीरता के बढ़ते क्रम में)
- `enum class LogCategory` — Untagged, Core, Rendering, Physics,
  Audio, Scripting, Networking, UI, AI, Resource — Memory के
  `MemoryTag` जैसा ही, ताकि पूरे इंजन में एक जैसी वर्गीकरण-भाषा
  (taxonomy) बनी रहे
- `struct LogMessage` — level, category, text, sourceFile, sourceLine,
  timestamp (यह `std::chrono::system_clock::now()` से अपने आप भर
  जाता है)
- `LogLevelToString()` / `LogCategoryToString()` — दोनों `constexpr`
  हैं, ताकि compile-time पर ही मूल्यांकित (evaluated) हो सकें, कोई
  रनटाइम लागत (runtime cost) न लगे

### `LogFormatter.h` / `.cpp`

`ILogFormatter` एक अमूर्त इंटरफ़ेस है, जिसका एकमात्र काम है
`LogMessage` को एक अंतिम, प्रिंट-योग्य string में बदलना। इस टैग में
सिर्फ़ एक ठोस implementation है — `DefaultLogFormatter`, जो इस तरह की
लाइन बनाता है:

```
[12:34:56] [INFO] [Rendering] Shader compiled (Renderer.cpp:42)
```

इंटरफ़ेस को अलग रखने का फ़ायदा यह है कि भविष्य में एक
`JsonLogFormatter` जोड़ा जा सकता है (structured logging के लिए, जैसे
किसी log-aggregation टूल के साथ इस्तेमाल के लिए) — बिना `Logger` या
`LogRegistry` में कुछ भी बदले।

`localtime_s`/`localtime_r` के बीच का चुनाव `#if defined(_WIN32)` से
किया जाता है — यह अभी से ही cross-platform सोच के साथ लिखा गया, भले ही
अभी सिर्फ़ Windows पर काम हो रहा है।

### `LogSink.h` / `.cpp`

`ISink` एक इंटरफ़ेस है जो यह तय करता है कि formatted संदेश आख़िर में
**कहाँ जाकर लिखा जाता है।** दो ठोस implementations हैं:

**`ConsoleSink`** — `Error` और `Fatal` स्तर के संदेश `stderr` पर जाते
हैं, बाकी सभी `stdout` पर। इससे console redirect करना और लॉग को
फ़िल्टर करना आसान हो जाता है (जैसे `program.exe 2> errors.txt`)।

**`FileSink`** — एक साधारण `std::ofstream` का इस्तेमाल करके फ़ाइल में
लिखता है, `std::ios::app` मोड में खुलता है (ताकि इंजन को दोबारा चलाने
पर पुराना लॉग मिट न जाए)। इसमें एक `TODO (Future)` comment है कि जब
`Core/FileSystem` बन जाएगा, तब इसे सीधे `std::ofstream` की बजाय उसी
प्रणाली के through फ़ाइल-पहुँच (file access) करनी चाहिए।

### `LogRegistry.h` / `.cpp`

यह क्लास तीन काम करती है:

1. **Sinks की सूची रखना** — `std::vector<std::unique_ptr<ISink>>`,
   इसमें कितने भी sinks जोड़े जा सकते हैं (Console + File दोनों
   एक साथ)
2. **Level Filtering** — `SetMinLevel()`/`GetMinLevel()`, जो भी संदेश
   इस स्तर से नीचे है, वह sinks तक पहुँचता ही नहीं
3. **Category Filtering** — हर `LogCategory` को अलग-अलग चालू/बंद
   (enable/disable) किया जा सकता है, `std::array<bool, ...>` के through

`Dispatch()` फ़ंक्शन पहले दोनों filters की जाँच करता है, और तभी
संदेश को `formatter.Format()` से formatted string में बदलकर हर sink
को भेजता है। पूरी क्लास `std::mutex` से सुरक्षित है।

### `Logger.h` / `.cpp`

यह पूरे मॉड्यूल का singleton, single entry point है। सबसे दिलचस्प
हिस्सा है इसके मैक्रो, जो `std::format` (C++20 का type-safe formatting
feature) का इस्तेमाल करते हैं:

```cpp
#define XR3D_LOG(level, category, fmt, ...) \
    ::XR3D::Core::Logging::Logger::Get().Log((level), (category), \
        std::format(fmt __VA_OPT__(,) __VA_ARGS__), __FILE__, __LINE__)

#define XR3D_LOG_INFO(category, fmt, ...)    XR3D_LOG(::XR3D::Core::Logging::LogLevel::Info,    category, fmt, __VA_ARGS__)
#define XR3D_LOG_ERROR(category, fmt, ...)   XR3D_LOG(::XR3D::Core::Logging::LogLevel::Error,   category, fmt, __VA_ARGS__)
```

`printf`-शैली की formatting (`%d`, `%s`) में एक जानी-मानी समस्या है —
अगर argument का type format-specifier से मेल न खाए, तो undefined
behaviour हो सकता है (कई बार crash तक)। `std::format` में यह समस्या
पूरी तरह नहीं होती, क्योंकि compiler खुद हर argument का सही type
जाँचता है। इसीलिए Logging को शुरू से ही `std::format`-आधारित बनाया
गया, `printf`-शैली नहीं।

`__VA_OPT__(,)` का इस्तेमाल इसलिए किया गया ताकि अगर कोई अतिरिक्त
argument न भी हो (सिर्फ़ एक स्थिर string हो), तब भी मैक्रो सही तरीके
से काम करे।

## उपयोग का उदाहरण

```cpp
#include "Core/Logging/Logger.h"

XR3D::Core::Logging::Logger::Get().Initialize();

XR3D_LOG_INFO(XR3D::Core::Logging::LogCategory::Rendering,
              "Frame {} rendered in {} ms", frameIndex, ms);

Logger::Get().GetRegistry().AddSink(std::make_unique<FileSink>("engine.log"));

XR3D::Core::Logging::Logger::Get().Shutdown();
```

## यूनिट टेस्टिंग — 6/6 पास

`Tests\Unit\Core\LoggingTests.cpp` में एक स्थानीय (local) सहायक क्लास
`RecordingSink` बनाई गई, जो `ISink` को implement करती है और सिर्फ़
आख़िरी संदेश और कुल गिनती याद रखती है — ताकि filtering व्यवहार को बिना
असली console/file को छुए जाँचा जा सके:

1. **`Logging_Logger_InitializeAndShutdown`** — दोहरा (double) init
   सुरक्षित है
2. **`Logging_DefaultFormatter_ContainsLevelCategoryAndText`** — एक
   `LogMessage` को खुद हाथ से बनाकर, `DefaultLogFormatter` से format
   करके, यह जाँचा गया कि परिणाम में level, category, text, और
   file:line सभी मौजूद हैं
3. **`Logging_FileSink_WritesToFile`** — `FileSink` से लिखकर, फिर
   फ़ाइल को वापस पढ़कर उसकी सामग्री (content) जाँची गई
4. **`Logging_Registry_CategoryFilter_BlocksDisabledCategory`** —
   Audio category को बंद करने पर उसका संदेश sink तक नहीं पहुँचता,
   लेकिन Rendering का पहुँचता है
5. **`Logging_Registry_LevelFilter_BlocksBelowMinLevel`** — minimum
   level को Error पर सेट करने पर Info संदेश रुक जाता है, Error गुज़र
   जाता है
6. **`Logging_Macro_FormatsArgumentsCorrectly`** — `XR3D_LOG_INFO`
   मैक्रो से भेजे गए `{}` placeholders सही मानों से भरे जाते हैं या
   नहीं, यह जाँचा गया

## इंटीग्रेशन टेस्टिंग — 3/3 पास

`Integration\Scenarios\Core\LoggingIntegrationTests.cpp`:

1. **`Integration_Logging_ConsoleAndFileSinkTogether`** — Console और
   File दोनों sinks एक साथ सक्रिय होने पर, दोनों जगह सही संदेश पहुँचते
   हैं
2. **`Integration_Logging_SimulatedFrameLoop_HighVolume`** — 200 नकली
   फ़्रेम, हर एक में दो categories में Trace-स्तर के log — यह जाँचता
   है कि उच्च-मात्रा (high-volume) logging क्रैश नहीं करती
3. **`Integration_Logging_RuntimeLevelAndCategoryToggling`** — इंजन
   चलते-चलते (बिना restart किए) verbosity level और category बदलना —
   जैसे कोई डेवलपर runtime पर एक console-command से verbosity बदले

उपयोगकर्ता (user) ने स्वयं तीनों targets (`XR3D_Engine`,
`XR3D_UnitTests`, `XR3D_Integration`) को एक साथ चलाकर पुष्टि की —
कुल 15 यूनिट टेस्ट (6 Logging + 9 Memory) और 7 इंटीग्रेशन सिनेरियो (3
Logging + 4 Memory), सभी सफल।

## Engine Integration

`Main.cpp` को इस टैग में दोबारा अपडेट किया गया — अब यह सीधे `printf`
की जगह `XR3D_LOG_INFO` इस्तेमाल करता है (यानी Logging मॉड्यूल खुद
अपने आप को "dogfood" कर रहा है — अपना ही बनाया हुआ tool इस्तेमाल कर
रहा है):

```cpp
Memory::MemoryManager::Get().Initialize();
Logging::Logger::Get().Initialize();

XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine starting...");
XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine running.");
XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine shutting down...");

Logging::Logger::Get().Shutdown();
Memory::MemoryManager::Get().Shutdown();
```

Initialize का क्रम **Memory पहले, फिर Logging** है (क्योंकि Logging
को अभी Memory की सीधी ज़रूरत नहीं, लेकिन भविष्य में हो सकती है)।
Shutdown का क्रम इसका उल्टा है — **Logging पहले, फिर Memory** — ताकि
Logger अपने आख़िरी संदेश फ्लश (flush) कर सके, इससे पहले कि Memory
पूरी तरह बंद हो जाए (Rule 43 — Initialization/Shutdown Contract)।

## सामने आई समस्याएँ

इस मॉड्यूल के Build Validation या Testing चरण में **कोई दोष (defect)
नहीं मिला।** पहली बार में ही सभी टेस्ट पास हुए। इसकी एक वजह यह भी है
कि Memory मॉड्यूल में जो सीख मिली (जैसे constructor में हमेशा
null-check करना, generic APIs को सही routing के साथ डिज़ाइन करना), वह
इस मॉड्यूल को लिखते वक़्त ध्यान में रखी गई।

## भविष्य के लिए बचे हुए काम (TODO)

- Fatal-स्तर के logs को `Core/Diagnostics` के through भेजना (जैसे एक
  crash-report तुरंत तैयार करना), एक बार वह प्रणाली बन जाए
- `FileSink` को `Core/FileSystem` के through माइग्रेट करना
- `Platform/Windows` बनने के बाद एक `PlatformDebugSink` जोड़ना (Visual
  Studio के Output window में भी log दिखे)
- ज़रूरत पड़ने पर `JsonLogFormatter` जोड़ना
- Sink-विशेष (per-sink) minimum level (जैसे Console सिर्फ़ Info+
  दिखाए, लेकिन File सब कुछ रिकॉर्ड करे)

## Git

```
git commit -m "feat(core): implement Logging subsystem (Console/File sinks, level+category filtering, std::format macros)"
git tag -a v0.3.0-logging -m "Core Logging module complete: 6 unit tests, 3 integration tests, wired into Engine"
```

## इस टैग से मिली सीख

- जब किसी मॉड्यूल का डिज़ाइन पिछले मॉड्यूल की सीख पर आधारित हो (जैसे
  यहाँ Memory से मिली सीख), तो implementation पहली बार में ही ज़्यादा
  ठोस (robust) बनती है — इसीलिए इस मॉड्यूल में शुरू से लेकर testing
  तक कोई bug नहीं मिला।
- `std::format` जैसे आधुनिक C++20 फ़ीचर, पुराने `printf`-शैली तरीकों
  से कहीं ज़्यादा सुरक्षित हैं, और इंजन के शुरुआती चरण में ही उन्हें
  अपनाना बेहतर है — बाद में migrate करना कहीं ज़्यादा मेहनत का काम
  होता।
- एक जैसा (consistent) पैटर्न अपनाना — जैसे `MemoryTag` और
  `LogCategory` को एक जैसी वर्गीकरण-भाषा देना — पूरे इंजन को समझने में
  आसान बनाता है, भले ही यह दो अलग-अलग enum हों।

---

<br>

<br>

# टैग 4 — `v0.4.0-config` (Core / Config मॉड्यूल)

## सारांश तालिका

| क्षेत्र | विवरण |
|---|---|
| टैग नाम | `v0.4.0-config` |
| संदेश | "Core Config module complete: 8 unit tests, 3 integration tests, wired into Engine" |
| मॉड्यूल समूह | Core |
| संस्करण चरण | V1 |
| Core में क्रम | 15 में से तीसरी प्रणाली |
| यूनिट टेस्ट | 8/8 पास |
| इंटीग्रेशन टेस्ट | 3/3 पास |

## यह मॉड्यूल क्यों तीसरे नंबर पर बनाया गया

Memory और Logging के तुरंत बाद Config बनाने का कारण यह था कि आगे बनने
वाली हर प्रणाली — Time, FileSystem, और आगे चलकर Rendering, Physics,
Audio जैसे feature मॉड्यूल — सभी को कुछ न कुछ **समायोज्य (tunable)
सेटिंग्स** चाहिए होंगी: विंडो का आकार, लक्ष्य फ़्रेम-रेट, गुरुत्वाकर्षण
(gravity), ध्वनि स्तर (volume), आदि। अगर यह मॉड्यूल पहले न बनाया जाता,
तो शुरुआती प्रणालियों में यह मान (values) सीधे कोड में लिखने पड़ते
(hardcode), जिन्हें बाद में Config प्रणाली में स्थानांतरित (migrate)
करना पड़ता — यह ठीक वही समस्या है जिससे "Advanced/Future-Proof" नियम
बचाना चाहता है।

## डिज़ाइन निर्णय — कस्टम टेक्स्ट फ़ॉर्मेट, JSON नहीं

Config मॉड्यूल के लिए एक महत्वपूर्ण फ़ैसला यह था कि फ़ाइल-फ़ॉर्मेट के
लिए **JSON जैसी किसी थर्ड-पार्टी लाइब्रेरी का इस्तेमाल नहीं किया
गया।** इसकी दो वजहें थीं:

1. परियोजना का नियम है कि किसी भी target में थर्ड-पार्टी लाइब्रेरी
   **डिफ़ॉल्ट रूप से नहीं** जोड़ी जाती — सिर्फ़ स्पष्ट कारण देकर,
   ज़रूरत पड़ने पर।
2. अभी इंजन को इतनी जटिल (complex) संरचना (nested objects, arrays)
   की ज़रूरत नहीं — एक साधारण "key = value" फ़ॉर्मेट काफ़ी है, और
   इसे हाथ से भी आसानी से संपादित (edit) किया जा सकता है।

इसलिए एक बहुत ही सरल, खुद लिखा हुआ parser बनाया गया, जो इस तरह की
फ़ाइलों को समझ सकता है:

```
# यह एक comment है
// यह भी comment है

window.width = 1920
window.fullscreen = false
audio.masterVolume = 0.8
player.name = "Ramakant"
```

## आर्किटेक्चर — पूरा चित्रण

```
                    ConfigManager  (singleton — एकमात्र बाहरी संपर्क बिंदु)
                          |
                    ConfigRegistry
        (thread-safe key-value store, std::unordered_map)
                          |
                    ConfigLoader
       (टेक्स्ट फ़ाइल और ConfigRegistry के बीच अनुवाद करता है)
```

## हर फ़ाइल की विस्तृत जानकारी

### `ConfigTypes.h` (header-only)

इस मॉड्यूल की सबसे दिलचस्प तकनीकी विशेषता यहीं है — `ConfigValue`:

```cpp
using ConfigValue = std::variant<bool, int64_t, double, std::string>;
```

`std::variant` (C++17 का फ़ीचर) एक ऐसा type है जो एक ही समय में चार में
से **कोई एक** प्रकार (bool, int64_t, double, या string) रख सकता है, और
यह पूरी तरह type-safe है — किसी `void*` या मैन्युअल टैगिंग (tagging)
की ज़रूरत नहीं। यह Rust जैसी आधुनिक भाषाओं में मिलने वाले "sum types"
या "tagged unions" जैसा concept है, C++ में `std::variant` के through
मिलता है।

`enum class ConfigValueType` सिर्फ़ रिपोर्टिंग/डिबगिंग के लिए है — यह
बताने के लिए कि किसी `ConfigValue` में असल में कौन-सा प्रकार सक्रिय
(active) है।

### `ConfigRegistry.h` / `.cpp`

यह क्लास एक साधारण thread-safe key-value store है:

- `Set(key, value)` — एक मान (value) सेट करना
- `Get(key)` — `std::optional<ConfigValue>` लौटाता है, ताकि यह साफ़
  पता चले कि key मौजूद है या नहीं (बिना कोई विशेष "not found" मान
  तय किए)
- `Has(key)` / `Remove(key)` / `Clear()` — साधारण operations
- `GetAllKeys()` — सभी keys की सूची (यह `ConfigLoader::SaveToFile()`
  के लिए ज़रूरी है, ताकि पूरी registry को फिर से फ़ाइल में लिखा जा
  सके)

पूरी क्लास `std::mutex` से सुरक्षित है, ताकि यह भविष्य में एक साथ कई
थ्रेड्स से इस्तेमाल होने पर भी सुरक्षित रहे — भले ही अभी इंजन single-
threaded है।

### `ConfigLoader.h` / `.cpp`

यह इस मॉड्यूल का सबसे बड़ा (technically) हिस्सा है, क्योंकि इसमें
असली parsing-logic है। इसके अंदर कुछ स्थानीय (local, anonymous
namespace में) सहायक फ़ंक्शन हैं:

**`Trim()`** — किसी string के शुरू और अंत से खाली जगह (whitespace)
हटाता है।

**`IsCommentOrBlank()`** — यह जाँचता है कि कोई line `#` या `//` से
शुरू होती है, या पूरी तरह खाली है — ऐसी lines को नज़रअंदाज़ (ignore)
किया जाता है।

**`LooksLikeInteger()` / `LooksLikeDouble()`** — यह तय करते हैं कि
कोई text एक पूर्णांक (integer) है या दशमलव संख्या (decimal number)।
यह character-by-character जाँच करके किया जाता है (हर character digit
है या नहीं, कहीं एक ही `.` तो नहीं है, आदि) — किसी regex या भारी
लाइब्रेरी के बिना।

**`StripQuotes()`** — अगर string की शुरुआत और अंत दोनों जगह `"` हो,
तो उन्हें हटा देता है, ताकि `"1.0.0"` जैसी value सिर्फ़ `1.0.0` के
रूप में स्टोर हो, quotes के साथ नहीं।

**`ParseValue()`** — इन सभी सहायक फ़ंक्शनों को मिलाकर तय करता है कि
किसी raw text को `bool`, `int64_t`, `double`, या `string` में से किस
प्रकार में बदला जाए। क्रम (order) महत्वपूर्ण है: पहले `true`/`false`
जाँचा जाता है, फिर पूर्णांक, फिर दशमलव, और अंत में — अगर कुछ भी मेल
न खाए — उसे string मान लिया जाता है।

**`LoadFromFile()`** — फ़ाइल को line-by-line पढ़ता है। हर line को
पहले trim किया जाता है, फिर comment/blank जाँचा जाता है, फिर `=` का
पहला (first) occurrence ढूँढकर key और value अलग किए जाते हैं। अगर
किसी line में `=` ही नहीं है, तो वह line **सिर्फ़ skip** हो जाती है
— **पूरी फ़ाइल load होना असफल नहीं होता,** सिर्फ़ वह एक line नज़रअंदाज़
होती है। यह एक जानबूझकर लिया गया डिज़ाइन-निर्णय है, क्योंकि config
फ़ाइलें अक्सर हाथ से संपादित (hand-edited) होती हैं, और एक छोटी टाइपो
(typo) की वजह से पूरी फ़ाइल का load fail होना बहुत ज़्यादा सख़्त
(strict) व्यवहार होता।

**`SaveToFile()`** — registry की सभी keys को एक-एक करके पढ़कर, हर
एक को `key = value` फ़ॉर्मेट में फ़ाइल में लिखता है।

### `ConfigManager.h` / `.cpp`

यह मॉड्यूल का singleton, single entry point है। इसकी सबसे दिलचस्प
विशेषता है इसका templated `GetOrDefault<T>()` फ़ंक्शन:

```cpp
template <typename T>
T GetOrDefault(const std::string& key, T defaultValue) const
{
    auto value = m_registry.Get(key);
    if (!value.has_value())
    {
        return defaultValue;
    }

    if (const T* typed = std::get_if<T>(&value.value()))
    {
        return *typed;
    }

    return defaultValue;
}
```

यह फ़ंक्शन दो अलग-अलग असफलता (failure) की स्थितियों को **कभी क्रैश
किए बिना** संभालता है:

1. **Key मौजूद ही नहीं है** — जैसे `"does.not.exist"` — तो सीधे
   `defaultValue` लौटा दिया जाता है।
2. **Key मौजूद है, लेकिन गलत प्रकार (type) माँगा गया** — जैसे किसी
   `int64_t` key को `bool` समझकर पढ़ने की कोशिश करना — तो
   `std::get_if<T>()` `nullptr` लौटाता है (क्योंकि variant के अंदर
   असल में active type कुछ और है), और यहाँ भी `defaultValue` लौटा
   दिया जाता है।

यह डिज़ाइन इसलिए महत्वपूर्ण है क्योंकि config फ़ाइलें इंसानों द्वारा
हाथ से लिखी/बदली जाती हैं — और इंसान गलतियाँ करते हैं (टाइपो, गलत
प्रकार, key का नाम बदल देना)। एक अच्छी Config प्रणाली को इन गलतियों
की वजह से पूरे इंजन को क्रैश नहीं होने देना चाहिए — बल्कि एक उचित
डिफ़ॉल्ट मान देकर आगे बढ़ जाना चाहिए।

## उपयोग का उदाहरण

```cpp
#include "Core/Config/ConfigManager.h"

XR3D::Core::Config::ConfigManager::Get().Initialize();

if (!ConfigManager::Get().LoadFromFile("engine.cfg"))
{
    // फ़ाइल नहीं मिली - defaults इस्तेमाल होंगे, कोई समस्या नहीं
}

int64_t width = ConfigManager::Get().GetOrDefault<int64_t>("window.width", 1280);

ConfigManager::Get().Set("audio.masterVolume", ConfigValue{ 0.8 });
ConfigManager::Get().SaveToFile("engine.cfg");

ConfigManager::Get().Shutdown();
```

## यूनिट टेस्टिंग — 8/8 पास

`Tests\Unit\Core\ConfigTests.cpp` में यह 8 टेस्ट हैं:

1. **`Config_Manager_InitializeAndShutdown`** — दोहरा (double) init
   सुरक्षित है
2. **`Config_SetAndGet_ReturnsCorrectTypedValue`** — चारों प्रकार
   (bool, int64_t, double, string) सेट और पढ़ने पर सही मान मिलते हैं
3. **`Config_GetOrDefault_ReturnsDefaultWhenMissing`** — गायब key के
   लिए default मान मिलता है
4. **`Config_GetOrDefault_ReturnsDefaultWhenTypeMismatch`** — गलत
   प्रकार माँगने पर default मान मिलता है, क्रैश नहीं होता
5. **`Config_Registry_HasAndRemove`** — `Has()` और `Remove()` सही
   काम करते हैं
6. **`Config_SaveAndLoad_RoundTrip`** — मान सेट करके फ़ाइल में
   सहेजना (save), फिर registry खाली करके दोबारा फ़ाइल से लोड करना —
   सभी मान बिल्कुल वैसे ही वापस मिलते हैं
7. **`Config_Loader_SkipsCommentsAndBlankLines`** — comments, खाली
   lines, और `=` रहित malformed lines सही तरीके से नज़रअंदाज़ होती
   हैं, valid keys फिर भी load होती हैं
8. **`Config_Loader_HandlesMissingFileGracefully`** — गायब फ़ाइल के
   लिए `LoadFromFile()` सिर्फ़ `false` लौटाता है, कोई exception नहीं
   फेंकता

## इंटीग्रेशन टेस्टिंग — 3/3 पास

`Integration\Scenarios\Core\ConfigIntegrationTests.cpp` में असली इंजन
जैसी परिस्थितियाँ बनाई गईं:

1. **`Integration_Config_LoadEngineSettingsAtStartup`** — एक असली
   जैसी "boot settings" फ़ाइल बनाकर, यह जाँचा गया कि कई अलग-अलग
   (नकली) उप-प्रणालियाँ अपनी-अपनी सेटिंग्स सही तरीके से पढ़ पाती हैं
2. **`Integration_Config_PersistUserSettingsAcrossRestarts`** — यह
   सबसे महत्वपूर्ण परिदृश्य (scenario) है: यह **दो अलग इंजन-सत्रों
   (sessions)** का अनुकरण (simulate) करता है — "सत्र 1" में इंजन
   बिना किसी फ़ाइल के शुरू होता है (पहली बार चलाना), खिलाड़ी (player)
   कुछ सेटिंग्स बदलता है, इंजन बंद होते वक़्त उन्हें फ़ाइल में सहेजता
   है। फिर "सत्र 2" में इंजन दोबारा शुरू होता है और पिछली बार सहेजी
   गई फ़ाइल को लोड करता है — यह बिल्कुल वैसा ही है जैसे कोई असली
   खिलाड़ी अपने गेम की सेटिंग्स (जैसे आवाज़ का स्तर, नियंत्रण
   सेटिंग्स) बदलता है और उम्मीद करता है कि अगली बार गेम खोलने पर वे
   सेटिंग्स याद रहें
3. **`Integration_Config_GracefulFallbackWhenConfigFileMissing`** —
   एक बिल्कुल नई स्थापना (fresh install) का अनुकरण करता है, जहाँ
   अभी तक कोई config फ़ाइल बनी ही नहीं है — यह जाँचता है कि इंजन फिर
   भी सामान्य रूप से, बिना क्रैश हुए, डिफ़ॉल्ट मानों के साथ शुरू हो
   जाता है

उपयोगकर्ता (user) ने स्वयं तीनों targets को एक साथ चलाकर पुष्टि की —
कुल 23 यूनिट टेस्ट (8 Config + 6 Logging + 9 Memory) और 10 इंटीग्रेशन
सिनेरियो (3 Config + 3 Logging + 4 Memory), सभी सफल।

## Engine Integration

`Main.cpp` को इस टैग में तीसरी बार अपडेट किया गया — अब यह Config को भी
शुरुआत (boot) प्रक्रिया में शामिल करता है:

```cpp
Memory::MemoryManager::Get().Initialize();
Logging::Logger::Get().Initialize();
Config::ConfigManager::Get().Initialize();

XR3D_LOG_INFO(Logging::LogCategory::Core, "XR3D Engine starting...");

if (!Config::ConfigManager::Get().LoadFromFile("engine.cfg"))
{
    XR3D_LOG_WARNING(Logging::LogCategory::Core,
        "engine.cfg not found - continuing with default settings.");
}

const int64_t windowWidth  = Config::ConfigManager::Get().GetOrDefault<int64_t>("window.width", 1280);
const int64_t windowHeight = Config::ConfigManager::Get().GetOrDefault<int64_t>("window.height", 720);

XR3D_LOG_INFO(Logging::LogCategory::Core, "Resolved window size: {}x{}", windowWidth, windowHeight);
```

इसमें एक महत्वपूर्ण बात ध्यान देने योग्य है: **Config, Logging के बाद
शुरू (initialize) किया जाता है**, ताकि अगर config फ़ाइल न मिले, तो
Config उस असफलता को **Logging के through रिपोर्ट कर सके** (जैसा ऊपर
"engine.cfg not found..." वाली warning line में दिख रहा है)। Shutdown
का क्रम इसका बिल्कुल उल्टा है — पहले **Config**, फिर **Logging** (ताकि
वह अपने आख़िरी संदेश फ्लश कर सके), और अंत में **Memory**।

असली Engine को चलाकर यह परिणाम देखा गया:

```
[INFO] [Core] XR3D Engine starting...
[WARNING] [Core] engine.cfg not found - continuing with default settings.
[INFO] [Core] Resolved window size: 1280x720
[INFO] [Core] XR3D Engine running.
[INFO] [Core] XR3D Engine shutting down...
```

यह दिखाता है कि तीनों Core प्रणालियाँ (Memory, Logging, Config) अब
एक साथ, सही क्रम में, बिना किसी क्रैश के काम कर रही हैं — और Config
की "graceful fallback" डिज़ाइन-सोच व्यवहार में भी सही साबित हुई
(फ़ाइल न मिलने पर भी इंजन सामान्य रूप से चला)।

## सामने आई समस्याएँ

इस मॉड्यूल के Build Validation या Testing चरण में **कोई दोष (defect)
नहीं मिला।** Memory और Logging से मिली सीख (जैसे हमेशा graceful
fallback सोचना, generic API को सही तरीके से route करना) यहाँ भी लागू
हुई, और नतीजा यह रहा कि यह लगातार तीसरा मॉड्यूल है जो बिना किसी बग के
पहली बार में ही पूरी तरह पास हुआ।

## भविष्य के लिए बचे हुए काम (TODO)

- फ़ाइल-एक्सेस को `Core/FileSystem` के through करना, अभी की तरह सीधे
  `std::ifstream`/`std::ofstream` इस्तेमाल करने की बजाय
- Hot-reload समर्थन जोड़ना (config फ़ाइल में बदलाव होने पर अपने आप
  दोबारा लोड होना), एक बार `Core/FileSystem` का file-watcher बन जाए
- `ConfigValueType::Vector3`/`Color` जोड़ना, एक बार इंजन के पास साझा
  (shared) गणितीय (math) प्रकार उपलब्ध हो जाएँ

## Git

```
git commit -m "feat(core): implement Config subsystem (variant-based key-value store, text file load/save)"
git tag -a v0.4.0-config -m "Core Config module complete: 8 unit tests, 3 integration tests, wired into Engine"
```

## इस टैग से मिली सीख

- `std::variant` जैसे आधुनिक C++17 फ़ीचर, पुराने `union`-आधारित
  तरीकों से कहीं ज़्यादा सुरक्षित हैं, और type-safe generic APIs
  (जैसे `GetOrDefault<T>()`) बनाने के लिए आदर्श हैं।
- किसी भी ऐसी प्रणाली को डिज़ाइन करते वक़्त जो **इंसानों द्वारा हाथ से
  संपादित की जाने वाली फ़ाइलों** पर निर्भर हो (जैसे config फ़ाइलें),
  "graceful degradation" (सफ़ाई से असफल होना, बिना क्रैश किए) एक
  अनिवार्य (mandatory) डिज़ाइन-सिद्धांत होना चाहिए — न कि एक बाद में
  जोड़ा जाने वाला "अतिरिक्त" फ़ीचर।
- तीन Core मॉड्यूल (Memory, Logging, Config) एक साथ मिलकर एक असली
  "बूट सीक्वेंस" (boot sequence) बनाते हैं — और यही वह नींव है जिस पर
  आगे की सभी 12 बची हुई Core प्रणालियाँ (Time, FileSystem, Threading,
  और आगे) खड़ी होंगी।

---

<br>

<br>

# टैग 5 — `v0.5.0-time` (Core / Time मॉड्यूल)

## सारांश तालिका

| क्षेत्र | विवरण |
|---|---|
| टैग नाम | `v0.5.0-time` |
| संदेश | "Core Time module complete: 10 unit tests, 3 integration tests, wired into Engine" |
| मॉड्यूल समूह | Core |
| संस्करण चरण | V1 |
| Core में क्रम | 15 में से चौथी प्रणाली |
| यूनिट टेस्ट | 10/10 पास |
| इंटीग्रेशन टेस्ट | 3/3 पास |

## यह मॉड्यूल क्यों चौथे नंबर पर बनाया गया

Time लगभग **हर भविष्य की प्रणाली** के लिए बुनियादी ज़रूरत है — Physics
को fixed-timestep चाहिए (deterministic simulation के लिए), Animation
को समय के साथ blending चाहिए, Rendering को frame-pacing चाहिए,
Networking को interpolation/timeout चाहिए, और gameplay logic को
cooldowns/buffs जैसी चीज़ें चाहिए। इसीलिए Memory, Logging, Config के
तुरंत बाद Time बनाना ज़रूरी था — और इसे **पूरी तरह एक साथ** (time-scale,
pause, fixed-step, safe timers — सब एक ही बार में) बनाना इसलिए ज़रूरी
था ताकि आगे चलकर इन फ़ीचर्स को Physics जैसी प्रणालियों में **बाद में
जबरदस्ती जोड़ना (retrofit)** न पड़े, जो कहीं ज़्यादा जोखिम भरा
(risky) होता।

## आर्किटेक्चर — पूरा चित्रण

```
                    TimeManager  (singleton — एकमात्र बाहरी संपर्क बिंदु)
                          |
              ------------------------
              |                      |
          TimeClock              TimeRegistry
   (delta-time, scale,        (सभी सक्रिय timers,
    pause, fixed-step)         generational handles के साथ)
                                     |
                                 TimeTimer
                            (एक अकेला timer instance)
```

## हर फ़ाइल की विस्तृत जानकारी

### `TimeTypes.h` (header-only)

`Clock` (= `std::chrono::steady_clock`), `TimePoint`, `Duration` (double
precision, सेकंड में) के type-aliases, और `enum class TimerType`
(OneShot/Repeating)। `steady_clock` जानबूझकर चुना गया — `system_clock`
की तुलना में यह कभी पीछे या आगे "जंप" नहीं करता (जैसे जब यूज़र सिस्टम
की घड़ी बदल दे), जो इसे frame-timing के लिए सुरक्षित बनाता है।

### `TimeHandle.h` (header-only)

`TimerHandle` — यह **generational handle** है, `index` + `generation`
दोनों रखता है। यह कॉन्सेप्ट पहले भी `ObjectHandle`/`ResourceHandle`
में सुझाया गया था, यहाँ पहली बार पूरी तरह लागू (implement) हुआ।

### `TimeTimer.h` / `.cpp`

एक अकेले timer का व्यवहार — `Configure()`, और `Advance(deltaSeconds)`
जो elapsed time बढ़ाता है और बताता है कि callback कितनी बार "due"
(फायर होने के लिए तैयार) है।

**यहाँ सबसे महत्वपूर्ण design-सिद्धांत है: `TimeTimer` खुद कभी अपना
callback नहीं बुलाता।** यह सिर्फ़ बताता है — "callback अब due है,
इतनी बार" — और असली firing की ज़िम्मेदारी `TimeRegistry` की है, जो इसे
तब करता है जब यह पूरी तरह सुरक्षित हो (नीचे देखें)।

### `TimeRegistry.h` / `.cpp`

यह क्लास सभी active timers को एक **slot-map** में रखती है — `index`
के through सीधे access, और `generation` counter हर slot ke साथ, ताकि
पुराने (stale) handles को नए (recycled) slot से अलग पहचाना जा सके।

**सबसे बड़ी design-चुनौती और उसका हल — Reentrancy/Deadlock Safety:**

सोचिए एक timer का callback खुद एक नया timer बनाता है (जैसे: एक
cooldown खत्म होने पर तुरंत एक follow-up effect शुरू करना — ये एक
बिल्कुल सामान्य gameplay pattern है)। अगर `UpdateAll()` अपने
`std::mutex` को lock किए हुए callback को सीधे बुला दे, और वो callback
अंदर से दोबारा `CreateTimer()` बुलाए (जो फिर से वही mutex lock करने
की कोशिश करेगा) — तो चूँकि `std::mutex` **recursive नहीं होता**, यह
**तुरंत deadlock** हो जाएगा — पूरा Engine वहीं रुक जाएगा।

इसे रोकने के लिए `UpdateAll()` को इस तरह डिज़ाइन किया गया:

```cpp
void TimeRegistry::UpdateAll(double deltaSeconds)
{
    std::vector<std::function<void()>> dueCallbacks;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // ... सभी timers advance करो, due callbacks को dueCallbacks
        //     list में collect karo, lekin abhi bulao mat ...
    } // <- lock yahan release ho jaata hai

    for (auto& callback : dueCallbacks)
    {
        callback(); // ab safe hai - mutex free hai
    }
}
```

Matlab: **lock ke andar sirf data-collect hota hai, callbacks lock
release hone ke BAAD fire hote hain।** इससे chahe callback kuch bhi
kare (naya timer banaye, purana destroy kare), koi deadlock nahi
hoga, aur registry ki state bhi corrupt nahi hogi (kyunki saari
mutations lock ke andar hi complete ho chuki hoti hain, firing shuru
hone se pehle)।

इसी class में एक और bug भी सामने आया aur turant fix hua (देखें
*Resolved Issues*) — **repeating timer ka multi-fire bug**।

### `TimeClock.h` / `.cpp`

यह इंजन की असली "घड़ी" है:

- **`Tick()`** — हर frame एक बार बुलाई जाती है, real wall-clock time
  से delta-time निकालती है
- **Delta-Time Clamping** — `m_maxDeltaTime` (default 0.25 सेकंड) से
  ज़्यादा कोई delta कभी नहीं जाने दिया जाता — "spiral of death" से
  बचाव (नीचे विस्तार से)
- **Time Scale** — `m_timeScale` से multiply करके scaled delta-time
  निकाला जाता है (slow-motion/fast-forward के लिए)
- **Pause** — pause होने पर scaled delta सीधे 0 हो जाता है (real समय
  फिर भी चलता रहता है, बस Engine को उसका पता नहीं चलता)
- **Fixed-Timestep Accumulator** — `m_fixedAccumulator` हर frame के
  scaled delta को जमा करता रहता है; `ConsumeFixedStep()` जब भी बुलाया
  जाए, अगर पर्याप्त समय जमा हो चुका है तो `true` लौटाता है और उतना
  समय घटा देता है — इसे loop में बुलाने से एक सामान्य **fixed-timestep
  game loop pattern** बनता है

**"Spiral of Death" क्या है और यह क्यों खतरनाक है:** मान लीजिए किसी
frame में debugger का breakpoint लग गया, या डिस्क धीमी हो गई, और वह
frame असल में 3 सेकंड ले लेता है। बिना clamping के, अगला `Tick()`
delta-time = 3.0 सेकंड रिपोर्ट करेगा। अगर Physics इस पूरे 3 सेकंड को
एक ही step में simulate करने की कोशिश करे, तो वस्तुएँ दीवारों के आर-पार
चली जाएँगी, collision detection टूट जाएगा — और अगले frame को भी उतना
ही समय लगेगा (क्योंकि उतनी भारी calculation करनी पड़ी), जिससे delta
और भी बड़ा हो जाएगा — यह एक **"spiral of death"** (मौत का चक्कर) बन
जाता है, जहाँ से इंजन कभी वापस उबर नहीं पाता। clamping (`m_maxDeltaTime`)
इसे रोकता है — इंजन बस "धीमा" महसूस होगा उस पल, क्रैश नहीं होगा।

### `TimeManager.h` / `.cpp`

पूरे मॉड्यूल का singleton — `TimeClock` और `TimeRegistry` दोनों को
owns करता है, और उनके बीच का पुल है: `Tick()` कॉल होने पर पहले clock
advance होता है, फिर registry के सभी timers उसी delta-time से advance
होते हैं।

## उपयोग का उदाहरण

```cpp
#include "Core/Time/TimeManager.h"

XR3D::Core::Time::TimeManager::Get().Initialize();

// हर frame:
TimeManager::Get().Tick();
double dt = TimeManager::Get().GetDeltaTime();

// Fixed-timestep Physics के लिए:
while (TimeManager::Get().ConsumeFixedStep())
{
    // PhysicsStep(TimeManager::Get().GetFixedDeltaTime());
}

// एक cooldown timer:
TimerHandle cooldown = TimeManager::Get().CreateTimer(2.0, TimerType::OneShot, []() {
    // ability ready हो गई
});

TimeManager::Get().Shutdown();
```

## यूनिट टेस्टिंग — 10/10 पास

`Tests\Unit\Core\TimeTests.cpp` में यह 10 टेस्ट हैं:

1. `Time_Manager_InitializeAndShutdown`
2. `Time_Clock_DeltaTimeIsPositiveAfterTick`
3. `Time_Clock_PauseSetsZeroDelta`
4. `Time_Clock_TimeScaleAffectsDelta`
5. `Time_Clock_MaxDeltaTimeClamps`
6. `Time_Timer_OneShot_FiresOnceAndCleansUp`
7. `Time_Timer_Repeating_FiresMultipleTimesForLargeDelta`
8. `Time_Timer_StaleHandle_DetectedAfterReuse`
9. `Time_FixedTimestep_ConsumeFixedStepWorks`
10. **`Time_Timer_ReentrantCallback_DoesNotDeadlock`** — यह सबसे
    महत्वपूर्ण टेस्ट है: एक timer का callback खुद एक नया timer बनाता
    है; अगर कहीं भी deadlock-bug होता, तो यह टेस्ट कभी खत्म ही नहीं
    होता (पूरा test-binary हैंग हो जाता) — इस टेस्ट का सफलतापूर्वक
    पूरा होना ही इस बात का प्रमाण है कि कोई deadlock नहीं है

**एक ध्यान देने योग्य बात:** चूँकि `TimeManager` एक process-भर का
singleton है, दो टेस्ट (`Time_Clock_MaxDeltaTimeClamps` और
`Time_FixedTimestep_ConsumeFixedStepWorks`) जानबूझकर अपनी ज़रूरत की
settings ख़ुद रीसेट/सेट करते हैं — क्योंकि एक टेस्ट में बदली गई सेटिंग
(जैसे `SetMaxDeltaTime`) अगर रीसेट न हो, तो वह अगले टेस्ट में "लीक"
(leak) हो सकती है, जो टेस्ट को गलत तरीके से fail करा सकती है। यह
**Time मॉड्यूल का दोष नहीं है** — यह टेस्ट-आइसोलेशन (isolation) की
सामान्य ज़िम्मेदारी है जब singleton state साझा (shared) होती है।

## इंटीग्रेशन टेस्टिंग — 3/3 पास

`Integration\Scenarios\Core\TimeIntegrationTests.cpp`:

1. **`Integration_Time_SimulatedGameLoop_FixedStepDrivesPhysics`** —
   10 नकली (~60 FPS) frames एक fixed-timestep accumulator को drive
   करते हैं, बिल्कुल वैसे ही जैसे असली Physics update loop भविष्य में
   करेगा
2. **`Integration_Time_AbilityCooldownSystem`** — तीन अलग-अलग
   duration के one-shot timers, छोटे-छोटे बार-बार होने वाले ticks से
   advance किए जाते हैं (जैसा असली cooldown UI हर frame करता है)
3. **`Integration_Time_PauseMenuScenario`** — एक repeating timer यह
   पुष्टि करता है कि `Pause()` सक्रिय रहते हुए वह **आगे नहीं बढ़ता**,
   और `Resume()` के बाद सही तरीके से दोबारा fire होना शुरू होता है

उपयोगकर्ता (user) ने स्वयं तीनों targets को एक साथ चलाकर पुष्टि की —
कुल 33 यूनिट टेस्ट (10 Time + 8 Config + 6 Logging + 9 Memory) और 13
इंटीग्रेशन सिनेरियो (3 Time + 3 Config + 3 Logging + 4 Memory), सभी
सफल।

## Engine Integration

`Main.cpp` को चौथी बार अपडेट किया गया — अब यह Time को भी शुरुआत (boot)
प्रक्रिया में शामिल करता है, aur ek demonstration frame-tick bhi
karta hai:

```cpp
Memory::MemoryManager::Get().Initialize();
Logging::Logger::Get().Initialize();
Config::ConfigManager::Get().Initialize();
Time::TimeManager::Get().Initialize();

// ... boot logic ...

Time::TimeManager::Get().Tick();
XR3D_LOG_INFO(Logging::LogCategory::Core, "First frame delta time: {} s", Time::TimeManager::Get().GetDeltaTime());

// Shutdown order: reverse - Time, Config, Logging, Memory.
Time::TimeManager::Get().Shutdown();
Config::ConfigManager::Get().Shutdown();
Logging::Logger::Get().Shutdown();
Memory::MemoryManager::Get().Shutdown();
```

असली Engine चलाने पर परिणाम:
```
[INFO] [Core] XR3D Engine starting...
[WARNING] [Core] engine.cfg not found - continuing with default settings.
[INFO] [Core] Resolved window size: 1280x720
[INFO] [Core] XR3D Engine running.
[INFO] [Core] First frame delta time: 0.010907 s
[INFO] [Core] XR3D Engine shutting down...
```

अब चारों Core प्रणालियाँ (Memory, Logging, Config, Time) एक साथ, सही
क्रम में, बिना किसी क्रैश के काम कर रही हैं।

## सामने आई समस्याएँ और उनका समाधान

इस मॉड्यूल में **दो चीज़ें** सामने आईं — दोनों development के दौरान ही
पकड़ी और ठीक की गईं, इसलिए final delivery पूरी तरह साफ़ थी:

1. **Repeating Timer का Multi-Fire Bug** — शुरुआती implementation में,
   अगर कोई बड़ा delta (जैसे `UpdateAll(0.35)` एक `0.1`-सेकंड वाले
   repeating timer पर) एक साथ कई intervals को पार कर जाए, तो timer
   सिर्फ़ **एक बार** fire होता था, तीन बार नहीं — matlab "beats" चुपचाप
   गायब (drop) हो जाते थे। यह sandbox-testing के दौरान ही पकड़ा गया
   (delivery से पहले), और `Advance()` को fire-**count** लौटाने वाला
   बनाकर ठीक किया गया।

2. **Test-Isolation Timing Flakiness** (मॉड्यूल का दोष नहीं) —
   `Integration_Time_SimulatedGameLoop_FixedStepDrivesPhysics` टेस्ट
   पहली बार `sleep_for()` की timing पर बहुत सख़्त (tight) upper-bound
   (`<= 12`) लगाए हुए था। Windows का scheduler/timer-resolution
   (ख़ासकर Debug build में, aur ek 200-line output wale Logging test
   ke turant baad) `sleep_for()` ko expected se zyada der tak sula
   sakta hai — jisse actual accumulated time zyada ho gaya aur physics
   steps 12 se upar chale gaye. Ye bound ko dheela (loose) karke
   (`>= 5 && <= 20`) theek kiya gaya — asli code me kuch nahi badla,
   sirf test ki tolerance realistic OS-timing-jitter ke against sahi
   ki gayi.

## भविष्य के लिए बचे हुए काम (TODO)

- `maxDeltaTime`/`fixedDeltaTime` को `Core/Config` के through
  configurable बनाना, एक बार वह integration point तैयार हो जाए
- `TimeUnit` enum (Seconds/Milliseconds/Frames) जोड़ना, अगर कभी
  formatting/conversion utility की ज़रूरत पड़े
- `TimeRegistry` की linear `UpdateAll()` scan को priority-queue/sorted
  structure में बदलना, अगर कभी concurrent timers की संख्या इतनी बढ़
  जाए कि यह एक मापी गई (measured) bottleneck बन जाए

## Git

```
git commit -m "feat(core): implement Time subsystem (clock, time-scale, pause, fixed-timestep, safe timers)"
git tag -a v0.5.0-time -m "Core Time module complete: 10 unit tests, 3 integration tests, wired into Engine"
```

## इस टैग से मिली सीख

- किसी भी ऐसे सिस्टम को डिज़ाइन करते वक़्त जिसमें **callback-आधारित
  event firing** हो aur जो एक साझा (shared) संसाधन (जैसे registry) पर
  lock ले रहा हो, हमेशा यह सोचना चाहिए: *"अगर यह callback खुद इसी
  सिस्टम को दोबारा बुलाए, तो क्या होगा?"* — reentrancy को शुरू से ही
  design में शामिल करना, बाद में deadlock ढूँढने से कहीं आसान है।
- Timer/scheduling जैसे systems में "एक बार में सिर्फ़ एक बार फायर
  होगा" जैसी सोच खतरनाक हो सकती है — असली दुनिया में frame-rate
  अनियमित (irregular) होती है, इसलिए हर API को **variable, कभी-कभी
  बड़े delta-time** के लिए सही तरीके से डिज़ाइन करना चाहिए, सिर्फ़
  "सामान्य" 16ms frame के लिए नहीं।
- Timing-आधारित टेस्ट (जो `sleep_for()` जैसे real-wall-clock पर
  निर्भर हों) कभी बहुत सख़्त bounds नहीं रखने चाहिए — OS scheduler
  jitter हमेशा मौजूद रहता है, ख़ासकर Debug builds में।

---

<br>

# आगे क्या (अगला टैग)

अगला मॉड्यूल **`Core\FileSystem`** होगा (Core की 15 प्रणालियों में
पाँचवाँ), जो फ़ाइल पढ़ने/लिखने, path-manipulation, aur file-watching
की ज़िम्मेदारी संभालेगा — यह अब तक जो भी `std::ifstream`/`std::ofstream`
directly इस्तेमाल हो रहा था (Logging का FileSink, Config का
ConfigLoader), उसे भविष्य में इसी प्रणाली के through migrate किया
जाएगा। जैसे ही वह पूरा होगा, इस दस्तावेज़ में **टैग 6 —
`v0.6.0-filesystem`** का पूरा विवरण जोड़ दिया जाएगा।

---

*यह दस्तावेज़ अंतिम बार अपडेट किया गया: टैग `v0.5.0-time` के बाद।*
*अगला अपडेट: टैग `v0.6.0-filesystem` पूरा होने पर।*