import React from "react";
import Strip from "./components/Strip";
import axios from "axios";
import { SketchPicker } from "react-color";

// function OldApp() {
//   const [color, setColor] = React.useState();
//   const [pixelColor, setPixelColor] = React.useState({
//     r: 0,
//     g: 0,
//     b: 0,
//     a: 1,
//   });
//   const [startPixel, setStartPixel] = React.useState(0);
//   const [endPixel, setEndPixel] = React.useState(144);
//   const handleFormSetPixelsColorSubmit = async (e) => {
//     e.preventDefault();
//     const { a, ...rgb } = pixelColor;
//     await axios.get("http://localhost:3000/", {
//       params: {
//         command: "setPixels",
//         s: startPixel,
//         e: endPixel,
//         ...rgb,
//       },
//     });
//     await axios.get("http://localhost:3000/", {
//       params: {
//         command: "brightness",
//         v: 255 * a,
//       },
//     });
//   };
//   const handleInputChange = (f) => (e) => f(e.target.value);
//   return (
//     <div>
//       <form onSubmit={handleFormSetPixelsColorSubmit}>
//         <h1>Set Pixels:</h1>
//         <SketchPicker
//           color={pixelColor}
//           onChange={(color) => setPixelColor(color.rgb)}
//         />
//         <label>
//           Start:
//           <input
//             value={startPixel}
//             type="number"
//             max="144"
//             onChange={handleInputChange(setStartPixel)}
//             name="startPixel"
//           />
//         </label>
//         <label>
//           End:
//           <input
//             value={endPixel}
//             type="number"
//             min="0"
//             max="144"
//             onChange={handleInputChange(setEndPixel)}
//             name="endPixel"
//           />
//         </label>
//         <button type="submit">Set Pixels</button>
//       </form>
//
//     </div>
//   );
// }

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
              axios.get("http://localhost:3000/", {
                params: {
                  command: "fill",
                  r,
                  g,
                  b,
                },
              }),
              axios.get("http://localhost:3000/", {
                params: {
                  command: "brightness",
                  v: 255 * a,
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
