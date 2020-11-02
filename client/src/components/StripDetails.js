import React from "react";
import { EFFECTS } from "../utils";

function StripDetailsCard({ selectedStrip, loading }) {
  if (loading) return "Loading";
  if (!selectedStrip.id) return "Strip not found";
  return (
    <div className="strip-card">
      <div className="strip-card__title">{selectedStrip.id}</div>
      <div className="strip-card__subtile"></div>
      <div className="strip-card__content">
        <div>Effect Status: {selectedStrip.effectActive ? "ON" : "OFF"}</div>
        <div>Selected Effect: {EFFECTS[selectedStrip.effect]}</div>
      </div>
    </div>
  );
}

function StripDetails(props) {
  return (
    <div className="strip-container">
      <StripDetailsCard {...props} />
    </div>
  );
}

export default StripDetails;
