# Telemetry mode:

| Code | Function |
| ------------- |:-------------:|
| 0   | Overview page regular check |  
| 1   | Quad 1 page regular check  |  
| 2   | Quad 2 page regular check  |
| 3   | Quad 3 page regular check  |
| 11  | Upload missions to quad 1 |
| 12  | Upload missions to quad 2 |
| 13  | Upload missions to quad 3 |
| 21  | Download missions from quad 1 |
| 22  | Download missions from quad 2 |
| 23  | Download missions from quad 3 |

# Manual mode:

| Code | Function |
| ------------- |:-------------:|
| 0   | Manual mode closed |  
| 1   | Manual control for quad 1 |  
| 2   | Manual control for quad 2 |
| 3   | Manual control for quad 3 |

# Remote control mode:

| Bit index | Function | Note |
|:-----:|:-----:|:-----:|
| 15 | Not used, hold for future usage | Highest bit |
| 14 | Not used, hold for future usage | N/A |
| 13 | Not used, hold for future usage | N/A |
| 12 | Not used, hold for future usage | N/A |
| 11 | Not used, hold for future usage | N/A |
| 10 | Not used, hold for future usage | N/A |
| 9  | RTH quad 3 | N/A |
| 8  | RTH quad 2 | N/A |
| 7  | RTH quad 1 | N/A |
| 6  | NAV quad 3 | N/A |
| 5  | NAV quad 2 | N/A |
| 4  | NAV quad 1 | N/A |
| 3  | ARM quad 3 | N/A |
| 2  | ARM quad 2 | N/A |
| 1  | ARM quad 1 | N/A |
| 0  | Remote control On/Off | Lowest bit |

### Examples:  
| Code | Function |
| ------------- |:-------------:|
| 0   | Remote control closed |  
| 1   | Remote control is open, but no RC command sending out |  
| 2   | Invalid |
| 3   | Remote control is open, quad 1 is armed |
| 4   | Invalid |
| ... | ... |
| 521 | Remote control is open, quad 3 is armed and on RTH mode |
| ... | ... |
| 911 | Remote control is open, all 3 quads on RTH mode |
| ... | ... |
| 1023| Invalid |
