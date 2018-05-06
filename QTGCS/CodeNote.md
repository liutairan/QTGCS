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
| 1023| Invalid |
