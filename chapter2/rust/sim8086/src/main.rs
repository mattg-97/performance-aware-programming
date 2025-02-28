use std::env;
use std::error::Error;
use std::fmt::{write, Display};
fn main() {
    let args: Vec<String> = env::args().collect();
    let file_name = args.get(1).unwrap();
    let mut bytes: Vec<u8> = Vec::new();
    match file_name.as_str() {
        "listing_0037" => {
            let read_bytes = include_bytes!("../../../asm/listing_0037");
            bytes = read_bytes.to_vec();
        }
        "listing_0038" => {
            let read_bytes = include_bytes!("../../../asm/listing_0038");
            bytes = read_bytes.to_vec();
        }
        _ => println!("You need to provide an asm file to disassemble"),
    }
    if let Err(e) = disassemble_instructions(&bytes) {
        dbg!(e);
    }
}

#[derive(Debug)]
pub enum Err {
    EmptyByteArray,
    InvalidOpCode,
    NoSecondByte,
}

impl Display for Err {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::EmptyByteArray => write!(
                f,
                "{}",
                "Empty byte array, please provide a file with the correc tinput"
            ),
            Self::InvalidOpCode => write!(f, "{}", "This opcode is unknown"),
            Self::NoSecondByte => write!(f, "{}", "Expecting a second byte"),
        }
    }
}

impl Error for Err {}

fn disassemble_instructions(bytes: &[u8]) -> Result<(), Err> {
    let b = match bytes.first() {
        Some(b) => b,
        None => return Err(Err::EmptyByteArray),
    };
    let opcode = (b >> 2) & 0x3F;
    match opcode {
        0b00100010 => disassemble_mov(bytes)?,
        _ => return Err(Err::InvalidOpCode),
    }
    Ok(())
}

fn disassemble_mov(bytes: &[u8]) -> Result<(), Err> {
    print!("mov ");
    let first_byte = match bytes.first() {
        Some(b) => b,
        None => return Err(Err::EmptyByteArray),
    };
    let d = (first_byte & 2) >> 1; // d = second to last bit
    let w = (first_byte & 1); // w = last bit

    let second_byte = match bytes.get(1) {
        Some(b) => b,
        None => return Err(Err::EmptyByteArray),
    };
    let md = (second_byte >> 6) & 3; // first 2 bits
    let reg = (second_byte >> 3) & 7; // middle 3 bits
    let rm = second_byte & 7;
    match md {
        0b11 => {
            print_reg(w, rm)?;
            print!(", ");
            print_reg(w, reg)?;
        }
        _ => return Err(Err::InvalidOpCode),
    }
    print!("\n");
    Ok(())
}

fn print_reg(w: u8, reg: u8) -> Result<(), Err> {
    match reg {
        0b000 => {
            if w == 0 {
                print!("AL");
            } else {
                print!("AX");
            }
        }
        0b001 => {
            if w == 0 {
                print!("CL");
            } else {
                print!("CX");
            }
        }
        0b010 => {
            if w == 0 {
                print!("DL");
            } else {
                print!("DX");
            }
        }
        0b011 => {
            if w == 0 {
                print!("BL");
            } else {
                print!("BX");
            }
        }
        0b100 => {
            if w == 0 {
                print!("AH");
            } else {
                print!("SP");
            }
        }
        0b101 => {
            if w == 0 {
                print!("CH");
            } else {
                print!("BP");
            }
        }
        0b110 => {
            if w == 0 {
                print!("DH");
            } else {
                print!("SI");
            }
        }
        0b111 => {
            if w == 0 {
                print!("BH");
            } else {
                print!("DI");
            }
        }
        _ => return Err(Err::InvalidOpCode),
    }
    Ok(())
}
