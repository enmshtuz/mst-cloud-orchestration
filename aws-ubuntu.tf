terraform {
  required_providers {
    aws = {
      source  = "hashicorp/aws"
      version = "~> 4.16"
    }
  }

  required_version = ">= 1.2.0"
}

provider "aws" {
  region = "us-east-1"
}

resource "aws_instance" "app_server" {
  ami           = "ami-0253407010935908c"
  instance_type = "t2.micro"
  key_name      = "TD1-AWS"

  tags = {
    Name = "MSTInstance"
  }

  vpc_security_group_ids = [aws_security_group.allow_ssh.id]

  user_data = <<-EOF
              #!/bin/bash
              apt-get update
              apt-get install -y build-essential libomp-dev
              # Add any additional setup or code execution steps here
              EOF
}

// Security Group allowing incoming SSH traffic
resource "aws_security_group" "allow_ssh" {
  name        = "allow_ssh"
  description = "Allow incoming SSH traffic"
  
  ingress {
    from_port = 22
    to_port   = 22
    protocol  = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}
