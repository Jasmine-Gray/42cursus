*This project has been created as part of the 42 curriculum by mishimod.*

## Description
**Net Practice** is a foundational project designed to introduce the core concepts of computer networking. The primary objective is to configure small-scale network topologies so that all specified hosts can communicate successfully with each other and the Internet. 

The project consists of 10 levels of increasing complexity, requiring a practical understanding of IP addressing, subnetting, and routing logic to achieve the "Congratulations!!" status for every goal.

## Instructions
### Running the Training Interface
1. Download and extract the project files into your desired directory.
2. Open the `index.html` file in a web browser. 
    * *Note: Google Chrome or a Chromium-based browser is strongly recommended for stability.*
3. Enter your intranet login in the provided field and select the "Training" tab to begin.

### Exporting Configurations
1. Once all goals for a level are satisfied, click the **"Get my config"** button to export your successful setup.
2. Save the exported configuration text into a dedicated file for each level.

### Submission Requirements
* You must submit **10 configuration files** (one per level) placed at the **root** of your Git repository.
* Ensure each file corresponds to its level (e.g., `level1`, `level2`, etc.).

## Resources
### Networking Concepts Studied
This project involved the study and application of the following concepts:
* **TCP/IP Addressing**: Assigning valid IP addresses within defined network segments.
* **Subnet Mask**: Using CIDR notation and decimal masks to define the size and boundaries of a subnetwork.
* **Default Gateway**: Configuring routers to direct traffic destined for outside the local network.
* **Routers and Switches**: Understanding how data-link (Layer 2) switching and network (Layer 3) routing facilitate communication.
* **OSI Layers**: Specifically focusing on the interactions at the Network (Layer 3) and Data Link (Layer 2) layers.

### AI Usage
AI (Gemini) was utilized as a collaborative tool to support the following tasks:
* **Troubleshooting**: Analyzing packet logs to diagnose `loop detected` and `No reverse way` errors, especially in Levels 9 and 10.
* **Subnet Calculation**: Verifying complex CIDR ranges (e.g., confirming that `.192/30` provides a specific 4-IP range) to ensure no overlap with fixed routes.
* **Routing Logic**: Clarifying the "Longest Prefix Match" principle to understand how specific routes take precedence over broader ones.

### Documentation & References
The following external resources were consulted to complete this project:
* [NetPractice 攻略メモ (note.com)](https://note.com/syamashi/n/n57fc506e0c5c)
* [42 Cursus Guide - NetPractice](https://42-cursus.gitbook.io/guide/4-rank-04/netpractice)
* [NetPractice Concept Presentation (Google Slides)](https://docs.google.com/presentation/d/1nMpZXMwcWWIHnJ_4GN5Y6Ccje_Cy9IeqwoC8ZRzgnh4/edit)
* [IP Addressing & Routing Presentation (Google Slides)](https://docs.google.com/presentation/d/1xh07srSD1IYBvJXEKQpsjdcEDWAGKnYQSS0iBO2Q0wU/edit)
* [IP Address Range Information (ipshu.com)](https://ja.ipshu.com/ip_d_list/192.168.41)
