import { DictUnit } from "@activejs/core";
import { navigate, Router } from "@reach/router";
import React from "react";
import api from "../api";
import StripDetails from "../components/StripDetails";
import "../styles/components/Strip.scss";

export const stripsListUnit = new DictUnit({
  initialValue: {
    ids: [],
    byID: {},
  },
});

export const updateStrip = (strip) => (current) => {
  console.log({ strip, current });
  return {
    ids: current.ids,
    byID: {
      ...current.byID,
      [strip.id]: strip,
    },
  };
};

export const reduceStripsList = (list, strip) => {
  return {
    ids: [...new Set([...list.ids, strip.id])],
    byID: { ...list.byID, [strip.id]: strip },
  };
};

export const getStripsList = async () => {
  try {
    const { data } = await api.get("/strips");
    const stripsList = data.reduce(reduceStripsList, stripsListUnit.value());
    stripsListUnit.dispatch(stripsList);
    return stripsList;
  } catch (error) {
    console.error(error);
    return {};
  }
};

function StripList({ children, location, ...props }) {
  const [loading, setLoading] = React.useState(false);
  const [strips, setStrips] = React.useState(stripsListUnit.value());
  const [selectedStrip, setSelectedStrip] = React.useState(-1);

  React.useEffect(() => {
    (async () => {
      try {
        setLoading(true);
        stripsListUnit.subscribe(setStrips);
        stripsListUnit.subscribe(console.log);
        const stripsList = await getStripsList();
        const { "*": stripId } = props;
        if (stripId) setSelectedStrip(stripId || -1);
      } catch (error) {
      } finally {
        setLoading(false);
      }
    })();
  }, []);

  const handleSelectStrip = (strip) => async () => {
    setSelectedStrip(strip.id);
    await navigate(strip.id);
  };

  return (
    <main className="dark-theme">
      <div className="strips-list-container">
        <ul className="strips-list">
          {strips.ids.map((stripId) => {
            const strip = strips.byID[stripId];
            return (
              <li
                className={`strips-list__item ${
                  selectedStrip === strip.id
                    ? "strips-list__item--selected"
                    : ""
                }`.trim()}
                key={strip.id}
                onClick={handleSelectStrip(strip)}
              >
                <span className="material-icons strips-list__item__icon">
                  wb_incandescent
                </span>
                <span>{strip.id}</span>
              </li>
            );
          })}
        </ul>
      </div>
      <Router className="strip-container">
        <StripDetails
          path="/:stripId"
          selectedStrip={strips.byID[selectedStrip]}
          loading={loading}
        />
      </Router>
    </main>
  );
}

export default StripList;
