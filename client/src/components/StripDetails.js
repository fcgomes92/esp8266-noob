import React from "react";
import { CustomPicker } from "react-color";
import Hue from "react-color/lib/components/common/Hue";
import Saturation from "react-color/lib/components/common/Saturation";
import api from "../api";
import { stripsListUnit, updateStrip } from "../pages/StripList";
import {
  buildColorStripURL,
  buildEffectStripURL,
  buildSpeedStripURL,
  EFFECTS,
} from "../utils";

function EffectSelector({ strip, loading, onChange }) {
  if (loading || !strip.id) return "loading...";
  async function handleChange(e) {
    const { value } = e.target;
    onChange(value);
  }
  return (
    <label className="tw-my-4 tw-block tw-uppercase tw-tracking-wide tw-text-gray-100 tw-text-xs tw-font-bold">
      Effect:
      <select
        value={strip.effect}
        onChange={handleChange}
        className="tw-block tw-appearance-none tw-w-full tw-bg-gray-900 tw-border tw-border-gray-400 tw-text-white tw-py-4 tw-px-4 tw-pr-8 tw-rounded tw-leading-tight tw-focus:outline-none tw-focus:bg-grey-900 tw-focus:border-gray-400"
      >
        {EFFECTS.map((e, idx) => (
          <option key={e.name} value={idx}>
            {e.name}
          </option>
        ))}
      </select>
    </label>
  );
}

function Pointer() {
  return (
    <div
      className="color-picker-pointer"
      style={{
        width: "18px",
        height: "18px",
        borderRadius: "50%",
        transform: "translate(-9px, -1px)",
        backgroundColor: "rgb(248, 248, 248)",
        boxShadow: "0 1px 4px 0 rgba(0, 0, 0, 0.37)",
      }}
    ></div>
  );
}

function _ColorSelector({ strip, loading, ...props }) {
  if (loading || !strip.id) return "loading...";
  return (
    <div className="color-picker">
      <div className="color-picker__hue">
        <Hue {...props} pointer={Pointer} />
      </div>
      <div className="color-picker__saturation">
        <Saturation {...props} pointer={Pointer} />
      </div>
    </div>
  );
}
const ColorSelector = CustomPicker(_ColorSelector);

function SpeedSelector({ strip, loading, onChange, ...props }) {
  if (loading || !strip.id) return "loading...";
  return (
    <div className="w-full md:w-1/2 px-3 mb-6 md:mb-0">
      <label className="tw-block tw-uppercase tw-tracking-wide tw-text-gray-100 tw-text-xs tw-font-bold tw-mb-2">
        Speed
        <input
          className="tw-appearance-none tw-block tw-w-full tw-bg-white tw-text-black tw-border tw-border-red-500 tw-rounded tw-py-3 tw-px-4 tw-mb-3 tw-leading-tight tw-focus:outline-none tw-focus:bg-white"
          onBlur={(e) => onChange(e.target.value)}
          defaultValue={strip.speed}
          type="number"
        />
      </label>
    </div>
  );
}

function StripDetailsCard({ selectedStrip, loading }) {
  if (loading) return "Loading";
  if (!selectedStrip || !selectedStrip.id) return "Strip not found";
  const handleEffectChange = async (effect) => {
    await api.get(
      buildEffectStripURL(selectedStrip.path, selectedStrip.id, effect)
    );
    stripsListUnit.dispatch(updateStrip({ ...selectedStrip, effect }));
  };

  const handleColorChange = async ({ rgb, hex }) => {
    api.get(buildColorStripURL(selectedStrip.path, selectedStrip.id, rgb));
    stripsListUnit.dispatch(updateStrip({ ...selectedStrip, color: hex }));
  };

  const handleSpeedChange = async (speed) => {
    api.get(buildSpeedStripURL(selectedStrip.path, selectedStrip.id, speed));
    stripsListUnit.dispatch(updateStrip({ ...selectedStrip, speed }));
  };

  return (
    <div className="strip-card tw-z-10 tw-shadow-xl">
      <div className="strip-card__title">{selectedStrip.id}</div>
      <div className="strip-card__subtile">{selectedStrip.path}</div>
      <div className="strip-card__content">
        <div>Speed (ms): {selectedStrip.speed}</div>
        <div>Color: {selectedStrip.color}</div>
        <hr className="tw-my-2" />
        <EffectSelector strip={selectedStrip} onChange={handleEffectChange} />
        <hr className="tw-my-2" />
        <ColorSelector
          strip={selectedStrip}
          onChange={handleColorChange}
          color={selectedStrip.color}
        />
        <hr className="tw-my-2" />
        <SpeedSelector strip={selectedStrip} onChange={handleSpeedChange} />
      </div>
    </div>
  );
}

function StripDetails(props) {
  return (
    <>
      {props.selectedStrip && props.selectedStrip.id && (
        <div
          className="strip-bg"
          style={{
            backgroundColor: props.selectedStrip.color,
          }}
        ></div>
      )}
      <StripDetailsCard {...props} />
    </>
  );
}

export default StripDetails;
