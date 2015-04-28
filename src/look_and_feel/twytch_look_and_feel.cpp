/* Copyright 2013-2015 Matt Tytel
 *
 * twytch is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * twytch is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with twytch.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "twytch_look_and_feel.h"
#include "twytch_slider.h"
#include "utils.h"

#define POWER_ARC_ANGLE 2.5

void TwytchLookAndFeel::drawLinearSlider(Graphics& g, int x, int y, int width, int height,
                                         float slider_pos, float min, float max,
                                         const Slider::SliderStyle style, Slider& slider) {
  static const DropShadow thumb_shadow(Colour(0x88000000), 3, Point<int>(-1, 0));

  bool bipolar = false;
  TwytchSlider* t_slider = dynamic_cast<TwytchSlider*>(&slider);
  if (t_slider)
    bipolar = t_slider->isBipolar();

  float pos = slider_pos - 1.0f;
  if (style == Slider::SliderStyle::LinearBar) {
    float h = slider.getHeight();

    g.setColour(Colour(0xff888888));
    if (bipolar)
      fillHorizontalRect(g, width / 2.0f, pos, h);
    else
      fillHorizontalRect(g, 0.0f, pos, h);

    thumb_shadow.drawForRectangle(g, Rectangle<int>(pos + 0.5f, 0, 2, h));
    g.setColour(slider.findColour(Slider::thumbColourId));
    g.fillRect(pos, 0.0f, 2.0f, h);
  }
  else if (style == Slider::SliderStyle::LinearBarVertical) {
    float w = slider.getWidth();

    g.setColour(Colour(0xff888888));
    if (bipolar)
      fillVerticalRect(g, height / 2.0f, pos, w);
    else
      fillVerticalRect(g, 0, pos, w);

    thumb_shadow.drawForRectangle(g, Rectangle<int>(0, pos + 0.5f, w, 2));
    g.setColour(slider.findColour(Slider::thumbColourId));
    g.fillRect(0.0f, pos, w, 2.0f);
  }
}

void TwytchLookAndFeel::drawLinearSliderThumb(Graphics& g, int x, int y, int width, int height,
                                              float slider_pos, float min, float max,
                                              const Slider::SliderStyle style, Slider& slider) {
  LookAndFeel_V3::drawLinearSliderThumb(g, x, y, width, height,
                                        slider_pos, min, max, style, slider);
}

void TwytchLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height,
                                         float slider_t, float start_angle, float end_angle,
                                         Slider& slider) {
  static const float stroke_percent = 0.12f;

  float full_radius = std::min(width / 2.0f, height / 2.0f);
  float stroke_width = 2.0f * full_radius * stroke_percent;
  float outer_radius = full_radius - stroke_width;
  PathStrokeType outer_stroke =
      PathStrokeType(stroke_width, PathStrokeType::beveled, PathStrokeType::butt);

  float knob_radius = 0.65f * full_radius;
  float current_angle = start_angle + slider_t * (end_angle - start_angle);
  float end_x = full_radius + 0.9f * knob_radius * sin(current_angle);
  float end_y = full_radius - 0.9f * knob_radius * cos(current_angle);

  if (slider.getInterval() == 1) {
    static const float TEXT_W_PERCENT = 0.35f;
    Rectangle<int> text_bounds(1.0f + width * (1.0f - TEXT_W_PERCENT) / 2.0f,
                               0.5f * height, width * TEXT_W_PERCENT, 0.5f * height);

    g.setColour(Colour(0xff464646));
    g.fillRect(text_bounds);

    g.setColour(slider.findColour(Slider::textBoxTextColourId));
    g.setFont(Font("Noto Sans", 0.24f * height, Font::plain));
    g.drawText(String(slider.getValue()), text_bounds,
               Justification::horizontallyCentred | Justification::bottom);
  }

  Path rail;
  rail.addCentredArc(full_radius, full_radius, outer_radius, outer_radius,
                     0.0f, start_angle, end_angle, true);

  g.setColour(Colour(0xff4a4a4a));
  g.strokePath(rail, outer_stroke);

  Path active_section;
  bool bipolar = false;
  TwytchSlider* t_slider = dynamic_cast<TwytchSlider*>(&slider);
  if (t_slider)
    bipolar = t_slider->isBipolar();

  if (bipolar) {
    active_section.addCentredArc(full_radius, full_radius, outer_radius, outer_radius,
                                 0.0f, 0.0f, current_angle - 2.0f * mopo::PI, true);
  }
  else {
    active_section.addCentredArc(full_radius, full_radius, outer_radius, outer_radius,
                                 0.0f, start_angle, current_angle, true);
  }

  g.setColour(Colour(0xffffab00));
  g.strokePath(active_section, outer_stroke);

  g.setColour(Colour(0xff000000));
  g.fillEllipse(full_radius - knob_radius, full_radius - knob_radius,
                2.0f * knob_radius, 2.0f * knob_radius);

  g.setColour(Colour(0xff666666));

  g.drawEllipse(full_radius - knob_radius + 1.0f, full_radius - knob_radius + 1.0f,
                2.0f * knob_radius - 2.0f, 2.0f * knob_radius - 2.0f, 2.0f);

  g.setColour(slider.findColour(Slider::rotarySliderFillColourId));
  g.drawLine(full_radius, full_radius, end_x, end_y, 1.0f);
}

void TwytchLookAndFeel::drawToggleButton(Graphics& g, ToggleButton& button,
                                         bool isMouseOverButton, bool isButtonDown) {
  static float stroke_percent = 0.1;

  float full_radius = std::min(button.getWidth(), button.getHeight()) / 2.0;
  float stroke_width = 2.0f * full_radius * stroke_percent;
  PathStrokeType stroke_type(stroke_width, PathStrokeType::beveled, PathStrokeType::rounded);
  float outer_radius = full_radius - stroke_width;
  Path outer;
  outer.addCentredArc(full_radius, full_radius, outer_radius, outer_radius,
                      mopo::PI, -POWER_ARC_ANGLE, POWER_ARC_ANGLE, true);
  if (button.getToggleState())
    g.setColour(Colours::white);
  else
    g.setColour(Colours::grey);
  
  g.strokePath(outer, stroke_type);
  g.fillRoundedRectangle(full_radius - 1.0f, 0.0f, 2.0f, full_radius, 1.0f);
}

void TwytchLookAndFeel::fillHorizontalRect(Graphics& g, float x1, float x2, float height) {
  float x = std::min(x1, x2);
  float width = fabsf(x1 - x2);
  g.fillRect(x, 0.0f, width, height);
}

void TwytchLookAndFeel::fillVerticalRect(Graphics& g, float y1, float y2, float width) {
  float y = std::min(y1, y2);
  float height = fabsf(y1 - y2);
  g.fillRect(0.0f, y, width, height);
}