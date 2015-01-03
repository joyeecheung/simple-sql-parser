## Test document for Compliers Course Project
#### Class 1 - group 10

| Name | Student ID | Job |
|--------|--------|--------|
| Fan Ziyao | 12330081 | Team leader, database implementation|
| Chen Zeyu | 12330056 | System implementation |
| Huang Long | 12330132 | Database implementation |
| Zhang Qiuyi (Class 2) | 12330402 | Frontend, documentation |
| Zhu Lichen (Class 2) | 12330439 | Testing |

### Create

#### #1 Normal create
![](screenshots/create1.png)

#### #2 Two or more columns with the same name
![](screenshots/create2.png)

#### #3 Multiple primary key definitions
![](screenshots/create3.png)

#### #4 Primary key is not defined as a column
![](screenshots/create4.png)

#### #5 Create an existing table
![](screenshots/create5.png)

### Insert

#### #1 Normal insert
![](screenshots/insert1.png)

#### #2 Duplicate column
![](screenshots/insert2.png)

#### #3 Insert with a column that is not in the schema
![](screenshots/insert3.png)

#### #4 Insert with differnt number of columns and values
![](screenshots/insert4.png)

#### #5 Key constraint violation
![](screenshots/insert5.png)

#### #6 Insert into a table that doesn't exist
![](screenshots/insert6.png)

#### #7 Insert without keys
![](screenshots/insert7.png)

#### #6 Insert with default values
![](screenshots/insert8.png)

### Query

#### #1 Select every columns and every rows
![](screenshots/query1.png)

#### #2 Select specific columns 1
![](screenshots/query2.png)

#### #3 Select specific columns 2
![](screenshots/query3.png)

#### #4 Select with no matching rows
![](screenshots/query4.png)

#### #5 Select with a simple where clause
![](screenshots/query6.png)

#### #7 Select a column that is not in the schema
![](screenshots/query7.png)

#### #8 Select with a where clause referencing a column not in the schema
![](screenshots/query8.png)

### Expressions
#### #1 Select with a complex where clause
![](screenshots/expr1.png)

#### #2 Select with a complex where clause
![](screenshots/expr2.png)

#### #3 Insert with a complex value
![](screenshots/expr3.png)

#### #4 Create a table with complex defaults
![](screenshots/expr4.png)

#### #5 Delete with a complex where clause
![](screenshots/expr5.png)

### Delete
#### #1 Delete with a simple where clause
![](screenshots/delete1.png)

#### #2 Delete all rows
![](screenshots/delete2.png)

#### #3 Delete with a where clause referencing a column not in the schema
![](screenshots/delete3.png)

### Division by zero

#### Division by zero in simple expressions
![](screenshots/zero1.png)

#### Division by zero in where clase
![](screenshots/zero2.png)

#### Divsion by zero in row evaluations
![](screenshots/zero3.png)

### Error Recoveries

#### Parse error
![](screenshots/error1.png)

#### Skip invalid lexeme
![](screenshots/error2.png)
