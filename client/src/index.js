import { Router } from "@reach/router";
import React from "react";
import ReactDOM from "react-dom";
import "regenerator-runtime/runtime";
import StripList from "./pages/StripList";
import * as serviceWorker from "./serviceWorker";
import "./styles/index.css";

ReactDOM.render(
  <React.StrictMode>
    <Router>
      <StripList path="/*" />
    </Router>
  </React.StrictMode>,
  document.getElementById("root")
);

// If you want your app to work offline and load faster, you can change
// unregister() to register() below. Note this comes with some pitfalls.
// Learn more about service workers: https://bit.ly/CRA-PWA
// serviceWorker.unregister();
