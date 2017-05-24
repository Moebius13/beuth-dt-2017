entity USB_PIN is
	port(SW:in bit_vector(0 to 3);
		check:in bit;
		pin_valid:out bit);
end entity USB_PIN;

architecture USB_PIN_a of USB_PIN is
	constant PIN:bit_vector(0 to 3) := "1010"
	process(check) begin
		if(check’event and check = ’0’ and check’last_value = ’1’) then
			with SW select
				pin_valid <= '1' when PIN,
					'0' when others;
		end if;
	end process;
end architecture USB_PIN_a;