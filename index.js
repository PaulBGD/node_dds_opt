const addon = require("bindings")("node_dds_opt");

const {Optimizer} = addon;

module.exports.Optimizer = Optimizer;
