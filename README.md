Overview
========

Meta-repository used by Jenkins CI and developers to check the interoperability between micrortps-client and
micrortps-agent is not break.

Usage
=====

This CMake meta-project is designed to clone micrortps-agent and micrortps-client in configuration phase. This meta-project expects four cmake arguments:

* CLIENT_URL: url of micrortps-client repository which will be tested.
* CLIENT_BRANCH: micrortps-client's branch which will be tested.
* AGENT_URL: url of micrortps-agent repository which will be tested.
* AGENT_BRANCH: micrortps-agent's branch which will be tested.

In the build phase it compiles both, micrortps-agent and micrortps-client, and executes a common tests created to check
integration between themselves.
