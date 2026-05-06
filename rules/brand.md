# Visual Branding

This workspace ships **without a default brand**. New plugins use stock JUCE `LookAndFeel_V4` unless the user supplies their own visual identity.

The `/dream` skill will ask about visual style during the UI concept step. When the user provides brand guidelines, document them per-plugin in `plugins/[PluginName]/.ideas/ui-mockup.md`, or — if the user wants a brand applied across every plugin they build — replace this file with their guidelines.

---

## What to capture when the user provides a brand

When the user describes their brand, record:

- **Company info**: name, 4-char manufacturer code, bundle ID prefix
- **Colors**: primary, accent, background, text, panel/surface (with hex values)
- **Fonts**: title, heading, body, monospace
- **Aesthetic direction**: dark vs light, minimal vs maximal, glass vs flat, etc.
- **Logo / header**: where branding appears in the plugin window

---

## Optional bundled fonts

The repo includes three fonts in `resources/fonts/` that any plugin can opt into:

- `Panchang-Medium.otf` — display / title font
- `Satoshi-Regular.otf` — body / labels
- `Satoshi-Bold.otf` — section headers

These are distributed by Indian Type Foundry under their free-for-commercial-use license — check the current terms at indiantypefoundry.com before redistributing. To use them in a plugin, add to that plugin's `CMakeLists.txt`:

```cmake
juce_add_binary_data(PluginNameFonts
    SOURCES
        ../../resources/fonts/Panchang-Medium.otf
        ../../resources/fonts/Satoshi-Regular.otf
        ../../resources/fonts/Satoshi-Bold.otf
)

target_link_libraries(PluginName PRIVATE PluginNameFonts)
```

Then load them via `juce::Typeface::createSystemTypefaceFor(BinaryData::PanchangMedium_otf, BinaryData::PanchangMedium_otfSize)`.

If you do not want to use these fonts, ignore them — they are not required.

---

## LookAndFeel pattern (when the user wants custom styling)

A typical custom brand defines:

```cpp
namespace MyBrand
{
    const juce::Colour accent      { 0x..., 0x..., 0x... };
    const juce::Colour background  { 0x..., 0x..., 0x... };
    // ...
}

class MyLookAndFeel : public juce::LookAndFeel_V4
{
public:
    MyLookAndFeel()
    {
        setColour(juce::Slider::rotarySliderFillColourId, MyBrand::accent);
        // ...
    }
};
```

Apply in the editor constructor with `setLookAndFeel(&lookAndFeel)` and clear it in the destructor with `setLookAndFeel(nullptr)`.
