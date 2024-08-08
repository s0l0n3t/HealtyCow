# HealtyCow Student Project
This repository created for TUBITAK 2209 - A/2 student supporting program.

 [![Build Status](https://static.licdn.com/aero-v1/sc/h/aahlc8ivbnmk0t3eyz8as5gvr)](https://www.linkedin.com/in/furkan-tokgoz/) 

## Thanks
*I am proud to share that my project "HealthyCow," which aims to detect diseases early and analyze health data in cattle farming, has been deemed suitable for support under the TUBITAK 2209-A/2 University Students Research Projects Support Program, in which I have taken on the role of a software developer.
I owe immense gratitude to our esteemed advisor, Associate Professor Dr. Ali NARİN, for his consultancy services in our project.
I would also like to extend my thanks to the valuable TUBITAK family and Professor Dr. Hasan Mandal for their role in making such a program possible and ensuring its sustainability.*

<img src="https://tubitak.gov.tr/sites/default/files/2023-08/logo.svg" 
        alt="Picture" 
        width="250" 
        height="250" 
        style="display: block; margin: 0 auto" />





## Server - Side

First of all, ***we need to run our belt.*** After that you need clone from repository.

```bash
git clone "https://github.com/s0l0n3t/HealtyCow.git"
```

Throughout open "/server" folder.

```bash
cd /HealtyCow/server/server
python server_.py
```

## Database - MySql

![github](/database_set.png)

```sql

CREATE TABLE data_table (
    id int NOT NULL AUTO_INCREMENT,
    device int(11),
    status int(11),
    temp varchar(100),
    pulse int(11)
);
```

## License

[Apache v2.0](https://choosealicense.com/licenses/apache-2.0/)
