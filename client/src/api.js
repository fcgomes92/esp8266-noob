import axios from "axios";
import makeConfig from "./config";

const api = axios.create({
  baseURL: makeConfig().apiURL,
});

export default api;
