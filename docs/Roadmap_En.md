
# Roadmap

## 1.7 or later

### Improve asset diversion + Provide a collection of materials

There is a shortage of human resources for the production of effects, and the number of people who can produce them is limited.
No matter what tools are used, there is a lot of knowledge required, which makes it tough for beginners.
In addition, there are only a small number of effects materials available for free use.

In response to these problems, in order to make it easier to create effects easily, we have developed the following tools to make it easier to create effects.
We will develop a system that allows users to create effects by combining parts in a tool or browser, and then output them for each environment.

A feature called SubEffect will be added to Effekseer to make it easier to combine parts.

The parts will be controllable with a few parameters, and users will be able to easily create basic effects by preparing a large number of parts needed for major effects.

In addition, a library of material data used for standard effects will be created and distributed license-free.
These data will also be distributed with the source data for editing, improving editability and solving the licensing problem of the unknown source of the material data.

### GPU Particles

GPU particles are supported to make it easier to display particles in large numbers.

### Mobile and Web Browser Optimization

Internal design will be reviewed and optimized on mobile and web browsers.

### WebGPU Support

Supports WebGPU, the next WebGL standard.

## 1.7

### Add Gradient feature

### Add trigger feature

You will be able to send external event triggers to the effects that during playback.
Depending on the trigger, the following behaviors can be defined on a node-by-node basis

- Stop particle generation by a node
- Start particle generation by a node
- Destroy of existing particles (with fade-out)

Previously, the only trigger actions were Stop or StopRoot.
It will be possible to influence effects more dynamically.

### Add various parameters