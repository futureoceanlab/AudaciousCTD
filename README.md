# AudaciousCTD

To understand any phenomenon about the ocean, we need to understand its basic
physical properties. Conductivity, tempreature and pressure are  
common data to be collected to estimate the local salinity of seawater at
certain depths. They are measured by using a sensor referred to as a 
conductivity, temperature and depth (CTD) sensor. It turns out that the existing
technologies are extremely costly due to their precision and depth ratings 
(a list of quates from various companies are available in 
**Markeg_Survey.xlsx**). For some scientific studies, the precision and depth 
rating specifications from these manufacturers are important. However, 
their high cost prevent us from understanding beyond local phenomenon in 
the ocean. 

Ocean is dynamic from molecular to global physical scales. Therefore, it is not
sufficient to operate with a few great CTDs. If their cost of a CTD could be 
lowered, we could have a more detailed understanding of the ocean. Furthermore,
the entry barrier for people to help study the ocean would be lowered 
significantly. We hope to acheive that via Audacious CTD project.

## Software

This was the last version of the MCU code used for the first iteration of the
Audacious CTD. It has not been maintained since June 2019.

The MCU used in this iteration was a STM32F303. 

The role of the MCU is very simple. Every given period, it will sample
the conductivity, temperature and pressure data from its sensors. Then, it will
save them to the flash memory. 

To edit the code, we recommend the use of STM32 Cube IDE. To program
the MCU, we recommend the use of STM32CubeProgrammer. This software also
enables the user to directly import the data that have been written to the 
flash drive. We took this approach in the first iteration.

## Circuit
The latest version of the circuit has been added to this repository. There
are two separte parts. One part has an MCU and its peripherals, and another
part has sensor-related components. The MCU component was housed inside, and
the sensor component was potted in the endcap to expose the sensors to the
seawater. 

At the point of fabrication, we thought having two separate board layouts (one 
for inside the housing and another for exposed circuit with sensors) on one 
board would save cost. However, it only caused confusion on the manufacturing 
company, and we would highly advise simply making two separte boards. 