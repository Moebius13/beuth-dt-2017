entity USB_PIN is
	port(SW:in bit_vector(0 to 3);
		pin_valid:out bit);
end entity USB_PIN;

architecture USB_PIN_a of USB_PIN is
	constant PIN:bit_vector(0 to 3) := "1010"
	process(SW) begin
		with SW select
			pin_valid <= '1' when PIN,
				'0' when others;
	end process;
end architecture USB_PIN_a;