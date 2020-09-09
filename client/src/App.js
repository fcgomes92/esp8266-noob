import React from "react";
import Strip from "./components/Strip";
import axios from "axios";
import { SketchPicker } from "react-color";

function App() {
  const [ledAmount, setLedAmount] = React.useState(144);
  const [fillColor, setFillColor] = React.useState();
  return (
    <div>
      <div>
        <h1>Fill:</h1>
        <SketchPicker
          color={fillColor}
          onChange={async ({ rgb: { r, g, b, a } }) => {
            setFillColor({ rgb: { r, g, b, a } });
            Promise.all([
              axios.get("http://localhost:3000/fill", {
                params: {
                  r,
                  g,
                  b,
                },
              }),
            ]);
          }}
        />
      </div>
      <label>
        Total number os LEDs:
        <input
          value={ledAmount}
          onChange={(e) => setLedAmount(Number(e.target.value))}
        />
      </label>
      <Strip total={ledAmount} />
    </div>
  );
}

export default App;
