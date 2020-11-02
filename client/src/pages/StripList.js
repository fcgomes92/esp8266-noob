import { navigate, Router } from "@reach/router";
import React from "react";
import api from "../api";
import StripDetails from "../components/StripDetails";
import "../styles/components/Strip.scss";

function StripList({ children, location, ...props }) {
  const [loading, setLoading] = React.useState(false);
  const [strips, setStrips] = React.useState([]);
  const [selectedStrip, setSelectedStrip] = React.useState({});

  React.useEffect(() => {
    (async () => {
      setLoading(true);
      try {
        const { data } = await api.get("/strips");
        setStrips(data);
        const { "*": stripId } = props;
        if (stripId) {
          setSelectedStrip(data.find((s) => s.id === stripId) || {});
        }
      } catch (error) {
        console.error(error);
      } finally {
        setLoading(false);
      }
    })();
  }, []);

  const handleSelectStrip = (strip) => async () => {
    setSelectedStrip(strip);
    await navigate(strip.id);
  };
  return (
    <main className="dark-theme">
      <div className="strips-list-container">
        <ul className="strips-list">
          {strips.map((strip) => {
            return (
              <li
                className={`strips-list__item ${
                  selectedStrip.id === strip.id
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
      <Router>
        <StripDetails
          path="/:stripId"
          selectedStrip={selectedStrip}
          loading={loading}
        />
      </Router>
    </main>
  );
}

export default StripList;
